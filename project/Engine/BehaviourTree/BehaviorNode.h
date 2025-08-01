#pragma once

// C++
#include <vector>
#include <memory>
#include <functional>

enum class BehaviorStatus {
	Success, // 成功
	Failure, // 失敗
	Running, // 実行中
};

/// <summary>
/// ノードの基底クラス
/// </summary>
template<typename AgentType>
class BehaviorNode {
public:
	virtual ~BehaviorNode() = default;

	/// <summary>
	/// ノードの状態を返す
	/// </summary>
	virtual BehaviorStatus Tick(AgentType* agent, float deltaTime) = 0;
};

/*---------------------------------------------------------------------------------*/
/*----------------------------------CompositeNode----------------------------------*/
/*---------------------------------------------------------------------------------*/

/// <summary>
/// コンポジット(複合)ノード : 複数の子ノードを持ち、どのように実行するかを制御する
/// </summary>
template<typename AgentType> 
class CompositeNodeBase : public BehaviorNode<AgentType> {
public:
	/// <summary>
	/// 子ノードをリストへ追加
	/// </summary>
	void AddChild(std::unique_ptr<BehaviorNode<AgentType>> node) { children_.push_back(std::move(node)); }

protected:
	/// <summary>
	/// 子ノードのリスト
	/// </summary>
	std::vector<std::unique_ptr<BehaviorNode<AgentType>>> children_;
};

/// <summary>
/// セレクタ(選択)ノード : 子ノードを順に評価し、最初に成功したノードで終了する
/// </summary>
template<typename AgentType> class SelectorNode : public CompositeNodeBase<AgentType> {
public:
	/// <summary>
	/// ノードの状態を返す
	/// </summary>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override {
		// 登録されている子ノードを先頭から順に評価
		for (auto& child : this->children_) {
			BehaviorStatus status = child->Tick(agent, deltaTime);
			// SuccessまたはRunningを返した子ノードが採用される
			if (status != BehaviorStatus::Failure) {
				return status;
			}
		}
		// すべての子ノードがFailureを返した場合、Failureを返す
		return BehaviorStatus::Failure;
	}
};

/// <summary>
/// シーケンス(連続)ノード : 子ノードを順番に評価し、すべて成功するまで継続する
/// </summary>
template<typename AgentType> class SequenceNode : public CompositeNodeBase<AgentType> {
public:
	/// <summary>
	/// ノードの状態を返す
	/// </summary>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override {
		// 登録されている子ノードを先頭から順に評価
		for (auto& child : this->children_) {
			BehaviorStatus status = child->Tick(agent, deltaTime);
			// Success以外を返した時点で、その結果を返す
			if (status != BehaviorStatus::Success) {
				return status;
			}
		}
		// 全ての子ノードがSuccessを返した場合、Successを返す
		return BehaviorStatus::Success;
	}
};

/*---------------------------------------------------------------------------------*/
/*----------------------------------DecoratorNode----------------------------------*/
/*---------------------------------------------------------------------------------*/

/// <summary>
/// デコレーターノード : 1つのみ子ノードを持ち、そのノードの実行結果の変化や制限を行う（※動作未確認）
/// </summary>
template<typename AgentType> 
class DecoratorNodeBase : public BehaviorNode<AgentType> {
public:
	/// <summary>
	/// 子ノードをセット
	/// </summary>
	void SetChild(std::unique_ptr<BehaviorNode<AgentType>> child) { child_ = std::move(child); }

	/// <summary>
	/// 子ノードの状態を返す
	/// </summary>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override {
		if (child_) {
			return child_->Tick(agent, deltaTime);
		}
		// 子ノードが設定されていなければ失敗
		return BehaviorStatus::Failure;
	}

protected:
	/// <summary>
	/// 子ノード（1つのみ）
	/// </summary>
	std::unique_ptr<BehaviorNode<AgentType>> child_;
};

/// <summary>
/// インバーター(反転)ノード（※動作未確認）
/// </summary>
template<typename AgentType> 
class InverterNode : public DecoratorNodeBase<AgentType> {
public:
	/// <summary>
	/// ノードの状態を返す
	/// </summary>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override { 
		// 子ノードの状態を取得し、SuccessならFailure, FailureならSuccessを返す
		BehaviorStatus status = DecoratorNodeBase<AgentType>::Tick(agent, deltaTime); // これできるか怪しい
		return (status == BehaviorStatus::Success) ? BehaviorStatus::Failure : BehaviorStatus::Success;
	}
};

/// <summary>
/// リピーター(繰り返し)ノード（※動作未確認）
/// </summary>
template<typename AgentType> 
class RepeaterNode : public DecoratorNodeBase<AgentType> {
public:
	/// <summary>
	/// ノードの状態を返す
	/// </summary>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override { 
		// 子ノードを繰り返し実行し、常に実行中を返す
		DecoratorNodeBase<AgentType>::Tick(agent, deltaTime); // これできるか怪しい
		return BehaviorStatus::Running;
	}

private:
	/// <summary>
	/// 繰り返し回数のカウンター（現在は未使用）
	/// </summary>
	uint32_t repeatCount = 0;
};

/// <summary>
/// アンティルサクセス(成功するまで)ノード（※動作未確認）
/// </summary>
template<typename AgentType> 
class UntilSuccessNode : public DecoratorNodeBase<AgentType> {
public:
	/// <summary>
	/// ノードの状態を返す
	/// </summary>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override { 
		while (true) {
			// 子ノードを実行し、成功するまで繰り返す
			BehaviorStatus status = DecoratorNodeBase<AgentType>::Tick(agent, deltaTime); // これできるか怪しい
			// Successなら終了
			if (status == BehaviorStatus::Success) {
				return BehaviorStatus::Success;
			}
			// Failureなら再度実行
			if (status == BehaviorStatus::Failure) {
				continue;
			}
			// Runningならそのまま返す
			return BehaviorStatus::Running;
		}
	}
};

/*---------------------------------------------------------------------------------*/
/*------------------------------------LeafNode------------------------------------*/
/*---------------------------------------------------------------------------------*/

/// <summary>
/// コンディション(条件)ノード : 特定の条件を評価し、その結果に応じてSuccessまたはFailureを返す
/// </summary>
template<typename AgentType> 
class ConditionNode : public BehaviorNode<AgentType> {
public:
	/// <summary>
	/// エージェントに基づいてbool値を返す関数
	/// </summary>
	using ConditionFunc = std::function<bool(AgentType*)>;

	/// <summary>
	/// 条件判定関数をコンストラクタで受け取って保持
	/// </summary>
	ConditionNode(ConditionFunc func) : conditionFunc_(func) {}

	/// <summary>
	/// ノードの状態を返す
	/// </summary>
	BehaviorStatus Tick(AgentType* agent, float) override { 
		// trueならSuccessを返し、falseならFailureを返す
		return conditionFunc_(agent) ? BehaviorStatus::Success : BehaviorStatus::Failure;
	}

private:
	ConditionFunc conditionFunc_;
};

/// <summary>
/// アクション(行動)ノード : 実際の行動
/// </summary>
template<typename AgentType> 
class ActionNode : public BehaviorNode<AgentType> {
public:
	/// <summary>
	/// アクション処理を行い、その状態を返す関数
	/// </summary>
	using ActionFunc = std::function<BehaviorStatus(AgentType*, float)>;

	/// <summary>
	/// 行動関数をコンストラクタで受け取って保持
	/// </summary>
	ActionNode(ActionFunc func) : actionFunc_(func) {}

	/// <summary>
	/// ノードの状態を返す
	/// </summary>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override { 
		return actionFunc_(agent, deltaTime);
	}

private: 
	ActionFunc actionFunc_;
};