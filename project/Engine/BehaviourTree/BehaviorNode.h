#pragma once

// C++
#include <vector>
#include <functional>

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

enum class BehaviorStatus {
	Success, // 成功
	Failure, // 失敗
	Running, // 実行中
};

/*---------------------------------------------------------------------------------*/
/*----------------------------------CompositeNode----------------------------------*/
/*---------------------------------------------------------------------------------*/

/// <summary>
/// コンポジット(複合)ノード : 複数の子ノードを持ち、どのように実行するかを制御する
/// </summary>
template<typename AgentType> 
class CompositeNode : public BehaviorNode<AgentType> {
public:
	/// <summary>
	/// 子ノードをリストへ追加
	/// </summary>
	void AddChild(BehaviorNode<AgentType>* node) { children_.push_back(node); }

protected:
	/// <summary>
	/// 子ノードのリスト
	/// </summary>
	std::vector<BehaviorNode<AgentType>*> children_;
};

/// <summary>
/// セレクタ(選択)ノード : 子ノードを順に評価し、最初に成功したノードで終了する
/// </summary>
template<typename AgentType> class SelectorNode : public CompositeNode<AgentType> {
public:
	/// <summary>
	/// ノードの状態を返す
	/// </summary>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override {
		// 登録されている子ノードを先頭から順に評価
		for (auto child : this->children_) {
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
template<typename AgentType> class SequenceNode : public CompositeNode<AgentType> {
public:
	/// <summary>
	/// ノードの状態を返す
	/// </summary>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override {
		// 登録されている子ノードを先頭から順に評価
		for (auto child : this->children_) {
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
		return conditionFunc_(agent) ? BehaviorStatus::Success : BehaviorStatus::Failure
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