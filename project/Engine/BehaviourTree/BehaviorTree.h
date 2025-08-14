#pragma once

// Engine
#include <Engine/BehaviourTree/BehaviorNode.h>

template<typename AgentType>
class BehaviorTree {
public:
	/// <summary>
	/// ルートノードをコンストラクタで受け取る
	/// </summary>
	BehaviorTree(std::unique_ptr<BehaviorNode<AgentType>> root) : root_(std::move(root)) {}

	/// <summary>
	/// ノードの状態を返す
	/// </summary>
	void Tick(AgentType* agent, float deltaTime) {
		if (root_) {
			root_->Tick(agent, deltaTime);
		}
	}

	/// <summary>
	/// ルートノードへの参照を取得
	/// </summary>
	BehaviorNode<AgentType>* GetRoot() {
		return root_.get();
	}

private:
	// ルートノードのリスト
	std::unique_ptr<BehaviorNode<AgentType>> root_;
};
