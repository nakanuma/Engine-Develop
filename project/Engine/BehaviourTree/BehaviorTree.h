#pragma once

// Engine
#include <Engine/BehaviourTree/BehaviorNode.h>

template<typename AgentType>
class BehaviorTree {
public:
	/// <summary>
	/// ルートノードをコンストラクタで受け取る
	/// </summary>
	BehaviorTree(BehaviorNode<AgentType>* root) : root_(root) {}

	/// <summary>
	/// ノードの状態を返す
	/// </summary>
	void Tick(AgentType* agent, float deltaTime) {
		if (root_) {
			root_->Tick(agent, deltaTime);
		}
	}

private:
	// ルートノードのリスト
	BehaviorNode<AgentType*> root_;
};
