#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/BehaviourTree/BehaviorNode.h>

namespace Cygnus {
// =========================================================
// ビヘイビアツリークラス
// : 全てのノードを管理
// =========================================================
template<typename AgentType> class BehaviorTree {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="root">ルートノード</param>
	BehaviorTree(std::unique_ptr<BehaviorNode<AgentType>> root) : root_(std::move(root)) {}

	/// <summary>
	/// ノードの状態を返します。
	/// </summary>
	/// <param name="agent">エージェントタイプ</param>
	/// <param name="deltaTime">デルタタイム</param>
	/// <returns>行動の状態</returns>
	void Tick(AgentType* agent, float deltaTime) {
		if (root_) {
			root_->Execute(agent, deltaTime);
		}
	}

	/// <summary>
	/// ツリー全体の構造を複製します。
	/// </summary>
	/// <returns></returns>
	std::unique_ptr<BehaviorTree<AgentType>> Clone() const {
		if(!root_) {
			return nullptr;
		}
		// ルートノードのCloneを呼び出す
		std::unique_ptr<BehaviorNode<AgentType>> newRoot = root_->Clone();
		// 複製されたルートを持つ新しいBehaviorTreeを作成して返す
		return std::make_unique<BehaviorTree<AgentType>>(std::move(newRoot));
	}

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// ルートノードへの参照を取得します。
	/// </summary>
	/// <returns>ルートノードへのポインタ</returns>
	BehaviorNode<AgentType>* GetRoot() { return root_.get(); }

private:
	// =========================================================
	// Member Variables
	// =========================================================

	std::unique_ptr<BehaviorNode<AgentType>> root_;			/* ルートノード */
};
}
