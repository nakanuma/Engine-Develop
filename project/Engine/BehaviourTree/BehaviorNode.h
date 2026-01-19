#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <vector>
#include <memory>
#include <functional>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/Util/RandomGenerator.h>

namespace Cygnus {
/// <summary>
/// 行動の状態を表す列挙型
/// </summary>
enum class BehaviorStatus {
	Success,		/* 成功 */
	Failure,		/* 失敗 */
	Running,		/* 実行中 */
};

// =========================================================
// ノードの基底クラス
// =========================================================
template<typename AgentType>
class BehaviorNode {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BehaviorNode() = default;

	/// <summary>
	/// 実行して状態を保存しつつノードの状態を返します。（Tickをラップ）
	/// </summary>
	/// <param name="agent"></param>
	/// <param name="deltaTime"></param>
	/// <returns></returns>
	BehaviorStatus Execute(AgentType* agent, float deltaTime) {
		lastStatus_ = Tick(agent, deltaTime);
		return lastStatus_;
	}

	/// <summary>
	/// 最後の状態を取得します。（エディターでの確認用）
	/// </summary>
	/// <returns></returns>
	BehaviorStatus GetLastStatus() const { return lastStatus_; }

	/// <summary>
	/// 最後の状態をリセットします。
	/// </summary>
	void ResetStatus() { lastStatus_ = BehaviorStatus::Running; }

	/// <summary>
	/// ノードの状態を返します。
	/// </summary>
	/// <param name="agent">エージェントタイプ</param>
	/// <param name="deltaTime">デルタタイム</param>
	/// <returns>行動の状態</returns>
	virtual BehaviorStatus Tick(AgentType* agent, float deltaTime) = 0;

	/// <summary>
	/// ノード名を取得します。
	/// </summary>
	/// <returns>ノード名（string）</returns>
	const std::string& GetName() const { return name_; }

	/// <summary>
	/// 自身のコピーしたインスタンスを返します。
	/// </summary>
	/// <returns></returns>
	virtual std::unique_ptr<BehaviorNode<AgentType>> Clone() const = 0;
 
protected:
	// =========================================================
	// Member Variables
	// =========================================================

	std::string name_ = "";		/* ノード名 */
	BehaviorStatus lastStatus_ = BehaviorStatus::Failure; /* 最後の状態を保存 */
};

/*---------------------------------------------------------------------------------*/
/*----------------------------------CompositeNode----------------------------------*/
/*---------------------------------------------------------------------------------*/

// =========================================================
// コンポジット(複合)ノード 
// : 複数の子ノードを持ち、どのように実行するかを制御する
// =========================================================
template<typename AgentType> 
class CompositeNodeBase : public BehaviorNode<AgentType> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="name">ノード名（string）</param>
	CompositeNodeBase(const std::string& name = "") { this->name_ = name; }

	/// <summary>
	/// 子ノードをリストへ追加します。
	/// </summary>
	/// <param name="node">追加する子ノード</param>
	void AddChild(std::unique_ptr<BehaviorNode<AgentType>> node) { children_.push_back(std::move(node)); }

	/// <summary>
	/// 子ノードのリストを取得します。
	/// </summary>
	/// <returns>子ノードのリスト</returns>
	const std::vector<std::unique_ptr<BehaviorNode<AgentType>>>& GetChildren() const { return children_; }

protected:
	/// <summary>
	/// 子ノードのリストを再帰的にコピーします。
	/// </summary>
	void CloneChildrenTo(CompositeNodeBase<AgentType>* newNode) const {
		for(const auto& child : children_) {
			newNode->AddChild(child->Clone());
		}
	}

protected:
	// =========================================================
	// Member Variables
	// =========================================================

	std::vector<std::unique_ptr<BehaviorNode<AgentType>>> children_;	/* 子ノードのリスト */
};

// =========================================================
// セレクタ(選択)ノード 
// : 子ノードを順に評価し、最初に成功したノードで終了する
// =========================================================
template<typename AgentType> class SelectorNode : public CompositeNodeBase<AgentType> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="name">ノード名（string）</param>
	SelectorNode(const std::string& name = "") : CompositeNodeBase<AgentType>(name) {}

	/// <summary>
	/// ノードの状態を返します。
	/// </summary>
	/// <param name="agent">エージェントタイプ</param>
	/// <param name="deltaTime">デルタタイム</param>
	/// <returns>行動の状態</returns>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override {
		// 登録されている子ノードを先頭から順に評価
		for (auto& child : this->children_) {
			BehaviorStatus status = child->Execute(agent, deltaTime);
			// SuccessまたはRunningを返した子ノードが採用される
			if (status != BehaviorStatus::Failure) {
				return status;
			}
		}
		// すべての子ノードがFailureを返した場合、Failureを返す
		return BehaviorStatus::Failure;
	}

	/// <summary>
	/// 自身のコピーしたインスタンスを返します。
	/// </summary>
	/// <returns></returns>
	virtual std::unique_ptr<BehaviorNode<AgentType>> Clone() const override {
		auto newNode = std::make_unique<SelectorNode<AgentType>>(this->name_);
		this->CloneChildrenTo(newNode.get());
		return newNode;
	}
};

// =========================================================
// シーケンス(連続)ノード 
// : 子ノードを順番に評価し、すべて成功するまで継続する
// =========================================================
template<typename AgentType> class SequenceNode : public CompositeNodeBase<AgentType> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="name">ノード名（string）</param>
	SequenceNode(const std::string& name = "") : CompositeNodeBase<AgentType>(name), currentIndex_(0) {}

	/// <summary>
	/// ノードの状態を返します。
	/// </summary>
	/// <param name="agent">エージェントタイプ</param>
	/// <param name="deltaTime">デルタタイム</param>
	/// <returns>行動の状態</returns>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override {
		while (currentIndex_ < this->children_.size()) {
			BehaviorStatus status = this->children_[currentIndex_]->Execute(agent, deltaTime);

			if (status == BehaviorStatus::Running) {
				return BehaviorStatus::Running; // 今の子で止める
			}
			if (status == BehaviorStatus::Failure) {
				currentIndex_ = 0; // リセット
				return BehaviorStatus::Failure;
			}

			// successなら次の子へ
			currentIndex_++;
		}

		// 全部成功したら
		currentIndex_ = 0;
		return BehaviorStatus::Success;
	}

	/// <summary>
	/// 自身のコピーしたインスタンスを返します。
	/// </summary>
	/// <returns></returns>
	virtual std::unique_ptr<BehaviorNode<AgentType>> Clone() const override {
		auto newNode = std::make_unique<SequenceNode<AgentType>>(this->name_);
		this->CloneChildrenTo(newNode.get());
		return newNode;
	}

private:
	// =========================================================
	// Member Variables
	// =========================================================

	size_t currentIndex_;		/* 現在実行中の子ノードのインデックス */
};

// =========================================================
// パラレル(平行)ノード 
// : 全ての子ノードを同時に実行する
// =========================================================
template<typename AgentType> class ParallelNode : public CompositeNodeBase<AgentType> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="name">ノード名（string）</param>
	ParallelNode(const std::string& name = "") : CompositeNodeBase<AgentType>(name) {}

	/// <summary>
	/// ノードの状態を返します。
	/// </summary>
	/// <param name="agent">エージェントタイプ</param>
	/// <param name="deltaTime">デルタタイム</param>
	/// <returns>行動の状態</returns>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override {
		bool allSuccess = true;
		bool anyRunning = false;

		for (auto& child : this->children_) {
			BehaviorStatus status = child->Execute(agent, deltaTime);

			if (status == BehaviorStatus::Failure) {
				// 1つでも失敗したら即Failureを返す
				return BehaviorStatus::Failure;
			} else if (status == BehaviorStatus::Running) {
				anyRunning = true;
				allSuccess = false;
			} else if (status == BehaviorStatus::Success) {
				// Successの場合は継続チェック
			}
		}

		if (allSuccess) {
			return BehaviorStatus::Success;
		}
		if (anyRunning) {
			return BehaviorStatus::Running;
		}

		return BehaviorStatus::Failure;
	}

	/// <summary>
	/// 自身のコピーしたインスタンスを返します。
	/// </summary>
	/// <returns></returns>
	virtual std::unique_ptr<BehaviorNode<AgentType>> Clone() const override {
		auto newNode = std::make_unique<ParallelNode<AgentType>>(this->name_);
		this->CloneChildrenTo(newNode.get());
		return newNode;
	}
};

/*---------------------------------------------------------------------------------*/
/*----------------------------------DecoratorNode----------------------------------*/
/*---------------------------------------------------------------------------------*/

// =========================================================
// デコレーターノード 
// : 1つのみ子ノードを持ち、そのノードの実行結果の変化や制限を行う
// =========================================================
template<typename AgentType> 
class DecoratorNodeBase : public BehaviorNode<AgentType> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 子ノードをセットします。
	/// </summary>
	/// <param name="child">子ノード</param>
	void SetChild(std::unique_ptr<BehaviorNode<AgentType>> child) { child_ = std::move(child); }

	/// <summary>
	/// 子ノードの状態を返します。
	/// </summary>
	/// <param name="agent">エージェント</param>
	/// <param name="deltaTime">デルタタイム</param>
	/// <returns>行動の状態</returns>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override {
		if (child_) {
			return child_->Execute(agent, deltaTime);
		}
		// 子ノードが設定されていなければ失敗
		return BehaviorStatus::Failure;
	}

	/// <summary>
	/// 保持している1つの子ノードをコピーします。
	/// </summary>
	/// <param name="newNode"></param>
	void CloneChildTo(DecoratorNodeBase<AgentType>* newNode) const {
		if(this->child_) {
			newNode->SetChild(this->child_->Clone());
		}
	}

protected:
	// =========================================================
	// Member Variables
	// =========================================================

	std::unique_ptr<BehaviorNode<AgentType>> child_; /* 子ノード（1つのみ） */
};

// =========================================================
// インバーター(反転)ノード
// : 子ノードの成功と失敗を反転させる
// =========================================================
template<typename AgentType> 
class InverterNode : public DecoratorNodeBase<AgentType> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 子ノードの状態を返します。
	/// </summary>
	/// <param name="agent">エージェント</param>
	/// <param name="deltaTime">デルタタイム</param>
	/// <returns>行動の状態</returns>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override { 
		// 子ノードの状態を取得し、SuccessならFailure, FailureならSuccessを返す
		BehaviorStatus status = DecoratorNodeBase<AgentType>::Execute(agent, deltaTime); // これできるか怪しい
		return (status == BehaviorStatus::Success) ? BehaviorStatus::Failure : BehaviorStatus::Success;
	}

	/// <summary>
	/// 自身のコピーを返します。
	/// </summary>
	/// <returns></returns>
	virtual std::unique_ptr<BehaviorNode<AgentType>> Clone() const override {
		auto newNode = std::make_unique<InverterNode<AgentType>>();
		newNode->name_ = this->name_;
		this->CloneChildTo(newNode.get());
		return newNode;
	}
};

// =========================================================
// リピーター(繰り返し)ノード
// : 子ノードを繰り返し実行し、常に実行中を返す
// =========================================================
template<typename AgentType> 
class RepeaterNode : public DecoratorNodeBase<AgentType> {
public:
	/// <summary>
	/// 子ノードの状態を返します。
	/// </summary>
	/// <param name="agent">エージェント</param>
	/// <param name="deltaTime">デルタタイム</param>
	/// <returns>行動の状態</returns>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override { 
		// 子ノードを繰り返し実行し、常に実行中を返す
		DecoratorNodeBase<AgentType>::Execute(agent, deltaTime); // これできるか怪しい
		return BehaviorStatus::Running;
	}

	/// <summary>
	/// 自身のコピーを返します。
	/// </summary>
	/// <returns></returns>
	virtual std::unique_ptr<BehaviorNode<AgentType>> Clone() const override {
		auto newNode = std::make_unique<RepeaterNode<AgentType>>();
		newNode->name_ = this->name_;
		this->CloneChildTo(newNode.get());
		return newNode;
	}

private:
	// =========================================================
	// Member Variables
	// =========================================================

	uint32_t repeatCount_ = 0; /* 繰り返し回数（現在は未使用） */
};

// =========================================================
// アンティルサクセス(成功するまで)ノード
// : 子ノードを成功するまで繰り返し実行する
// =========================================================
template<typename AgentType> 
class UntilSuccessNode : public DecoratorNodeBase<AgentType> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 子ノードの状態を返します。
	/// </summary>
	/// <param name="agent">エージェント</param>
	/// <param name="deltaTime">デルタタイム</param>
	/// <returns>行動の状態</returns>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override { 
		while (true) {
			// 子ノードを実行し、成功するまで繰り返す
			BehaviorStatus status = DecoratorNodeBase<AgentType>::Execute(agent, deltaTime); // これできるか怪しい
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

	/// <summary>
	/// 自身のコピーを返します。
	/// </summary>
	/// <returns></returns>
	virtual std::unique_ptr<BehaviorNode<AgentType>> Clone() const override {
		auto newNode = std::make_unique<UntilSuccessNode<AgentType>>();
		newNode->name_ = this->name_;
		this->CloneChildTo(newNode.get());
		return newNode;
	}
};

/*---------------------------------------------------------------------------------*/
/*------------------------------------LeafNode------------------------------------*/
/*---------------------------------------------------------------------------------*/

// =========================================================
// コンディション(条件)ノード 
// : 特定の条件を評価し、その結果に応じてSuccessまたはFailureを返す
// =========================================================
template<typename AgentType> 
class ConditionNode : public BehaviorNode<AgentType> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// エージェントに基づいてbool値を返す関数
	/// </summary>
	using ConditionFunc = std::function<bool(AgentType*)>;

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="func">条件判定関数</param>
	/// <param name="name">ノード名</param>
	ConditionNode(ConditionFunc func, const std::string& name = "") : conditionFunc_(std::move(func)) 
	{
		this->name_ = name;
	}

	/// <summary>
	/// ノードの状態を返します。
	/// </summary>
	/// <param name="agent">エージェント</param>
	/// <param name="deltaTime">デルタタイム</param>
	/// <returns>行動の状態</returns>
	BehaviorStatus Tick(AgentType* agent, float) override { 
		// trueならSuccessを返し、falseならFailureを返す
		return conditionFunc_(agent) ? BehaviorStatus::Success : BehaviorStatus::Failure;
	}

	/// <summary>
	/// 自身のクローンを返します。
	/// </summary>
	/// <returns></returns>
	virtual std::unique_ptr<BehaviorNode<AgentType>> Clone() const override {
		return std::make_unique<ConditionNode<AgentType>>(this->conditionFunc_, this->name_);
	}

private:
	// =========================================================
	// Member Variables
	// =========================================================

	ConditionFunc conditionFunc_;		/* 条件判定関数 */
};

// =========================================================
// アクション(行動)ノード
// : 実際の行動を定義するノード
// =========================================================
template<typename AgentType> 
class ActionNode : public BehaviorNode<AgentType> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// アクション処理を行い、その状態を返す関数
	/// </summary>
	using ActionFunc = std::function<BehaviorStatus(AgentType*, float)>;

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="func">行動関数</param>
	/// <param name="name">ノード名</param>
	ActionNode(ActionFunc func, const std::string& name = "") : actionFunc_(std::move(func))
	{
		this->name_ = name;
	}

	/// <summary>
	/// ノードの状態を返します。
	/// </summary>
	/// <param name="agent">エージェント</param>
	/// <param name="deltaTime">デルタタイム</param>
	/// <returns>行動の状態</returns>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override { 
		return actionFunc_(agent, deltaTime);
	}

	/// <summary>
	/// 自身のクローンを返します。
	/// </summary>
	/// <returns></returns>
	virtual std::unique_ptr<BehaviorNode<AgentType>> Clone() const override {
		return std::make_unique<ActionNode<AgentType>>(this->actionFunc_, this->name_);
	}

private: 
	// =========================================================
	// Member Variables
	// =========================================================

	ActionFunc actionFunc_;			/* 行動関数 */
};

// =========================================================
// 待機ノード
// : 指定された時間だけ待機するノード
// =========================================================
template<typename AgentType>
class WaitNode : public BehaviorNode<AgentType> {
public:
	// =========================================================
	// Public Methods
	// =========================================================
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="minWaitTime">最小待機時間</param>
	/// <param name="maxWaitTime">最大待機時間</param>
	/// <param name="name">ノード名</param>
	WaitNode(float minWaitTime, float maxWaitTime, const std::string& name = "") : minWaitTime_(minWaitTime), maxWaitTime_(maxWaitTime), elapsedTime_(0.0f), isWaiting_(false)
	{
		this->name_ = name;
	}

	/// <summary>
	/// ノードの状態を返します。
	/// </summary>
	/// <param name="agent">エージェント</param>
	/// <param name="deltaTime">デルタタイム</param>
	/// <returns>行動の状態</returns>
	BehaviorStatus Tick(AgentType* agent, float deltaTime) override {
		// 評価時に引数で受け取った値か
		if (!isWaiting_) {
			waitTime_ = RandomGenerator::GetInstance()->RandomValue(minWaitTime_, maxWaitTime_);
			elapsedTime_ = 0.0f;
			isWaiting_ = true;
		}

		elapsedTime_ += deltaTime;

		if (elapsedTime_ >= waitTime_) {
			// 次の評価時に備えてリセット
			isWaiting_ = false;
			return BehaviorStatus::Success;
		}
		return BehaviorStatus::Running;
	}

	/// <summary>
	/// 自身のクローンを返します。
	/// </summary>
	/// <returns></returns>
	virtual std::unique_ptr<BehaviorNode<AgentType>> Clone() const override {
		return std::make_unique<WaitNode<AgentType>>(this->minWaitTime_, this->maxWaitTime_, this->name_);
	}

private:
	// =========================================================
	// Member Variables
	// =========================================================

	float minWaitTime_;			/* 最小待機時間 */
	float maxWaitTime_;			/* 最大待機時間 */

	float waitTime_;			/* 実際に待機する時間 */
	float elapsedTime_;			/* 経過時間 */

	bool isWaiting_;			/* 待機中フラグ */
};
}