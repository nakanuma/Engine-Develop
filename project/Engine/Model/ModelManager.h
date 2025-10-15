#pragma once
#include <d3d12.h>
#include <map>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// MyClass
#include "MyMath.h"
#include "TextureManager.h"

/// <summary>
/// モデル管理クラス
/// </summary>
class ModelManager {
public:
	/// <summary>
	/// 頂点データ構造体
	/// </summary>
	struct VertexData {
		Float4 position;
		Float2 texcoord;
		Float3 normal;
	};

	/// <summary>
	/// マテリアル情報
	/// </summary>
	struct MaterialData {
		std::string textureFilePath;
		uint32_t textureHandle;
	};

	/// <summary>
	/// ノード構造体
	/// </summary>
	struct Node {
		QuaternionTransform transform;
		Matrix localMatrix;
		std::string name;
		std::vector<Node> children;
	};

	/// <summary>
	/// 頂点に対するジョイントの影響情報
	/// </summary>
	struct VertexWeightData {
		float weight;
		uint32_t vertexIndex;
	};

	/// <summary>
	/// ジョイントごとのスキニング情報
	/// </summary>
	struct JointWeightData {
		Matrix inverseBindPoseMatrix;
		std::vector<VertexWeightData> vertexWeights;
	};

	/// <summary>
	/// モデル全体のデータ構造
	/// </summary>
	struct ModelData {
		std::map<std::string, JointWeightData> skinClusterData;
		std::vector<VertexData> vertices;
		std::vector<uint32_t> indices;
		MaterialData material;
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		Node rootNode;
	};

	///
	///	Primitive
	///

	// リングの生成
	static ModelData CreateRingModel(ID3D12Device* device, float outerRadius = 1.0f, float innerRadius = 0.2f);
	// シリンダーの生成（memo : このモデルを描画する際はPSOをNoCullingに設定）
	static ModelData CreateCylinderModel(ID3D12Device* device);
	// スカイボックスの生成
	static ModelData CreateSkyBoxModel(ID3D12Device* device);

	///
	/// Load
	///

	// Objファイルの読み込みを行う
	static ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename, ID3D12Device* device);
	// mtlファイルの読み込みを行う
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename, ID3D12Device* device);
	// assimpのNodeから、Node構造体に変換
	static Node ReadNode(aiNode* node);
};
