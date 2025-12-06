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
			root_->Tick(agent, deltaTime);
		}
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
