#pragma once

// externals
#include <ImguiWrapper.h>
#include <externals/nlohmann/json.hpp>

// Engine
#include <Engine/BehaviourTree/BehaviorTree.h>

// C++
#include <memory>

/// <summary>
/// BehaviorTreeEditor
/// </summary>
template<typename AgentType>
class BehaviorTreeEditor
{
public:
	BehaviorTreeEditor() {
		context_ = ImNodes::CreateContext();
	}

	~BehaviorTreeEditor() {
		if (context_) {
			ImNodes::DestroyContext(context_);
			context_ = nullptr;
		}
	}

	/// <summary>
	/// 内部ノードデータの生成
	/// </summary>
	void SetBehaviorTree(BehaviorTree<AgentType>* tree) {
		tree_ = tree;
		nodes_.clear();
		if (tree_ && tree->GetRoot()) {
			uint32_t startID = 1;
			BuildNodeView(tree->GetRoot(), startID, nodes_);
		}
	}

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() {
		ImNodes::SetCurrentContext(context_);

		ImNodes::BeginNodeEditor();

		// ノード描画
		for (auto& node : nodes_) {
			ImNodes::BeginNode(node.id);

			// タイトル
			ImNodes::BeginNodeTitleBar();
			ImGui::Text("%s", node.name.c_str());
			ImNodes::EndNodeTitleBar();

			// サブタイトル
			if(!node.subName.empty()) {
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", node.subName.c_str());
				ImGui::Separator();
			}

			// 入力ピン
			ImNodes::BeginInputAttribute(node.id * 10 + 0);
			ImGui::Text("In");
			ImNodes::EndInputAttribute();

			// 出力ピン
			ImNodes::BeginOutputAttribute(node.id * 10 + 1);
			ImGui::Text("Out");
			ImNodes::EndOutputAttribute();

			ImNodes::EndNode();

			// 位置を反映
			ImNodes::SetNodeEditorSpacePos(node.id, node.position);
		}

		// リンク描画
		for (auto& node : nodes_) {
			for (uint32_t childID : node.childrenIDs) {
				uint32_t linkID = node.id * 1000 + childID;

				ImNodes::Link(linkID, node.id * 10 + 1, childID * 10 + 0);
			}
		}

		ImNodes::EndNodeEditor();

		// ノード位置の更新
		for (auto& node : nodes_) {
			node.position = ImNodes::GetNodeEditorSpacePos(node.id);
		}
	}

	/// <summary>
	/// セーブ
	/// </summary>
	void Save(const std::string& path) {
		std::string fullPath = defaultDir_ + path;

		nlohmann::json j;
		for (auto& node : nodes_) {
			j[std::to_string(node.id)] = { node.position.x, node.position.y };
		}

		std::ofstream file(fullPath);
		file << j.dump(4);
	}
	
	/// <summary>
	/// ロード
	/// </summary>
	void Load(const std::string& path) {
		std::string fullPath = defaultDir_ + path;

		std::ifstream file(fullPath);
		if (!file.is_open()) return;
		nlohmann::json j;
		file >> j;

		for (auto& node : nodes_) {
			if (j.contains(std::to_string(node.id))) {
				auto pos = j[std::to_string(node.id)];
				node.position = ImVec2(pos[0], pos[1]);
			}
		}
	}

private:
	struct NodeView {
		uint32_t id; // ノード固有ID
		std::string name; // ノード名
		std::string subName; // サブ名
		ImVec2 position; // ノード位置
		std::vector<uint32_t> childrenIDs; // 子ノードのIDリスト（リンク描画に使用）
	};
	// 描画用ノード配列
	std::vector<NodeView> nodes_;
	// 対象のツリー
	BehaviorTree<AgentType>* tree_ = nullptr;

	ImNodesContext* context_ = nullptr;
	std::string defaultDir_ = "resources/Configs/BehaviorTree/";
	std::string fileName_ = "";

	/// <summary>
	/// ノードビュー生成
	/// </summary>
	void BuildNodeView(BehaviorNode<AgentType>* node, uint32_t& currentID, std::vector<NodeView>& nodes) {
		if (!node) return;

		NodeView view;
		view.id = currentID++;

		// ノードタイプに応じた名前を設定
		if (auto selector = dynamic_cast<SelectorNode<AgentType>*>(node)) {
			view.name = "Selector";
			view.subName = selector->GetName();
		}
		else if (auto sequence = dynamic_cast<SequenceNode<AgentType>*>(node)) {
			view.name = "Sequence";
			view.subName = sequence->GetName();
		}
		else if (auto paralell = dynamic_cast<ParallelNode<AgentType>*>(node)) {
			view.name = "Parallel";
			view.subName = paralell->GetName();
		}
		else if (auto condition = dynamic_cast<ConditionNode<AgentType>*>(node)) {
			view.name = "Condition";
			view.subName = condition->GetName();
		}
		else if (auto action = dynamic_cast<ActionNode<AgentType>*>(node)) {
			view.name = "Action";
			view.subName = action->GetName();
		}
		else if (auto wait = dynamic_cast<WaitNode<AgentType>*>(node)) {
			view.name = "Wait";
			view.subName = wait->GetName();
		}

		// CompositeNodeなら子ノードも再帰
		if (auto composite = dynamic_cast<CompositeNodeBase<AgentType>*>(node)) {
			for (auto& child : composite->GetChildren()) {
				uint32_t childID = currentID;
				BuildNodeView(child.get(), currentID, nodes);
				view.childrenIDs.push_back(childID);
			}
		}

		nodes.push_back(view);
	}
};
