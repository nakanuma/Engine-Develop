#pragma once

// ---------------------------------------------------------
// Externals Includes
// ---------------------------------------------------------
#include <externals/nlohmann/json.hpp>

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <memory>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/BehaviourTree/BehaviorTree.h>
#include <ImguiWrapper.h>

namespace Cygnus {
// =========================================================
// ビヘイビアツリーエディタークラス
// =========================================================
template<typename AgentType> class BehaviorTreeEditor {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	BehaviorTreeEditor() {
#ifdef USE_IMGUI
		context_ = ImNodes::CreateContext();
#endif
	}

	/// <summary>
	/// デストラクタ
	/// </summary>
	~BehaviorTreeEditor() {
#ifdef USE_IMGUI
		// ノードデータをクリア
		nodes_.clear();
		tree_ = nullptr;

		// コンテキストを破棄
		if (context_) {
			ImNodes::DestroyContext(context_);
			context_ = nullptr;
		}
#endif
	}

	/// <summary>
	/// ビヘイビアツリーを設定します。
	/// </summary>
	/// <param name="tree">ビヘイビアツリー</param>
	void SetBehaviorTree(BehaviorTree<AgentType>* tree) {
		tree_ = tree;
		nodes_.clear();
		if (tree_ && tree->GetRoot()) {
			uint32_t startID = 1;
			// 内部ノードデータの生成
			BuildNodeView(tree->GetRoot(), startID, nodes_);
		}
	}

	/// <summary>
	/// エディターの描画処理を行います。
	/// </summary>
	void Draw() {
#ifdef USE_IMGUI
		ImNodes::SetCurrentContext(context_);
		ImNodes::BeginNodeEditor();

		// ノード描画
		for (auto& node : nodes_) {
			// 状態に応じた色の決定
			ImColor nodeColor = ImColor(60, 60, 60); // デフォルト（グレー）
			if(node.nodePtr) {
				switch(node.nodePtr->GetLastStatus()) {
					case BehaviorStatus::Success: nodeColor = ImColor(40, 200, 40); break; // 緑
					case BehaviorStatus::Failure: nodeColor = ImColor(200, 40, 40); break; // 赤
					case BehaviorStatus::Running: nodeColor = ImColor(255, 200, 0); break; // 黄
				}
			}

			// ノードのタイトルバーと枠線の色を変える
			ImNodes::PushColorStyle(ImNodesCol_TitleBar, nodeColor);
			ImNodes::PushColorStyle(ImNodesCol_PinHovered, nodeColor);
			ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, nodeColor);

			ImNodes::BeginNode(node.id);

			// タイトル
			ImNodes::BeginNodeTitleBar();
			ImGui::Text("%s", node.name.c_str());
			ImNodes::EndNodeTitleBar();

			// サブタイトル
			if (!node.subName.empty()) {
				ImGui::TextColored(kSubTitleColor, "%s", node.subName.c_str());
				ImGui::Separator();
			}

			// 入力ピン
			ImNodes::BeginInputAttribute(node.id * kInputAttributeMultiplier + kInputAttributeIndex);
			ImGui::Text("In");
			ImNodes::EndInputAttribute();

			// 出力ピン
			ImNodes::BeginOutputAttribute(node.id * kOutputAttributeMultiplier + kOutputAttributeIndex);
			ImGui::Text("Out");
			ImNodes::EndOutputAttribute();

			ImNodes::EndNode();

			// 位置を反映
			ImNodes::SetNodeEditorSpacePos(node.id, node.position);
		}

		// リンク描画
		for (auto& node : nodes_) {
			for (uint32_t childID : node.childrenIDs) {
				uint32_t linkID = node.id * kLinkMultiplier + childID;

				ImNodes::Link(linkID, node.id * kOutputAttributeMultiplier + kOutputAttributeIndex, childID * kInputAttributeMultiplier + kInputAttributeIndex);
			}
		}

		ImNodes::EndNodeEditor();

		// ノード位置の更新
		for (auto& node : nodes_) {
			node.position = ImNodes::GetNodeEditorSpacePos(node.id);
		}
#endif
	}

	/// <summary>
	/// エディターのコンフィグファイルを保存します。
	/// </summary>
	/// <param name="path">ファイルパス</param>
	void Save(const std::string& path) {
		std::string fullPath = kDefaultDir + path;

		nlohmann::json j;
		for (auto& node : nodes_) {
			j[std::to_string(node.id)] = {node.position.x, node.position.y};
		}

		std::ofstream file(fullPath);
		file << j.dump(4);
	}

	/// <summary>
	/// エディターのコンフィグファイルをロードします。
	/// </summary>
	/// <param name="path">ファイルパス</param>
	void Load(const std::string& path) {
		std::string fullPath = kDefaultDir + path;

		std::ifstream file(fullPath);
		if (!file.is_open())
			return;
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
	/// <summary>
	/// 1つ分のノード可視化情報を保持する構造体
	/// </summary>
	struct NodeView {
		uint32_t id;                       /* ノード固有ID */
		std::string name;                  /* ノード名 */
		std::string subName;               /* サブノード名 */
		ImVec2 position;                   /* ノード位置 */
		std::vector<uint32_t> childrenIDs; /* 子ノードのIDリスト（リンク描画に使用） */
		BehaviorNode<AgentType>* nodePtr;  /* ノードへのポインタ */
	};

	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// ノードビューを生成します。
	/// </summary>
	/// <param name="node">ノード</param>
	/// <param name="currentID">現在のID</param>
	/// <param name="nodes">ノードリスト</param>
	void BuildNodeView(BehaviorNode<AgentType>* node, uint32_t& currentID, std::vector<NodeView>& nodes) {
		if (!node)
			return;

		NodeView view;
		view.id = currentID++;
		view.nodePtr = node;

		// ノードタイプに応じた名前を設定
		if (auto selector = dynamic_cast<SelectorNode<AgentType>*>(node)) {
			view.name = "Selector";
			view.subName = selector->GetName();
		} else if (auto sequence = dynamic_cast<SequenceNode<AgentType>*>(node)) {
			view.name = "Sequence";
			view.subName = sequence->GetName();
		} else if (auto parallel = dynamic_cast<ParallelNode<AgentType>*>(node)) {
			view.name = "Parallel";
			view.subName = parallel->GetName();
		} else if (auto condition = dynamic_cast<ConditionNode<AgentType>*>(node)) {
			view.name = "Condition";
			view.subName = condition->GetName();
		} else if (auto action = dynamic_cast<ActionNode<AgentType>*>(node)) {
			view.name = "Action";
			view.subName = action->GetName();
		} else if (auto wait = dynamic_cast<WaitNode<AgentType>*>(node)) {
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

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr uint32_t kInputAttributeMultiplier = 10;	/* 入力ピンIDを生成するための乗数 */
	static constexpr uint32_t kOutputAttributeMultiplier = 10;	/* 出力ピンIDを生成するための乗数 */
	static constexpr uint32_t kLinkMultiplier = 1000;			/* リンクIDを生成するための乗数 */
	static constexpr uint32_t kInputAttributeIndex = 0;			/* 入力ピンのインデックス */
	static constexpr uint32_t kOutputAttributeIndex = 1;		/* 出力ピンのインデックス */

	static constexpr ImVec4 kSubTitleColor = ImVec4{0.0f, 1.0f, 1.0f, 1.0f};		/* サブタイトルのデフォルトカラー */
	static constexpr const char* kDefaultDir = "resources/Configs/BehaviorTree/";	/* デフォルトの保存ディレクトリ */

	// =========================================================
	// Member Variables
	// =========================================================

	std::vector<NodeView> nodes_;             /* 描画用ノード配列 */
	BehaviorTree<AgentType>* tree_ = nullptr; /* 対象のビヘイビアツリー */

	ImNodesContext* context_ = nullptr;                          /* ImNodesコンテキスト */
	std::string fileName_ = "";                                  /* 保存ファイル名 (拡張子含む) */
};
}
