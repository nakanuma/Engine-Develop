#pragma once
#include <vector>
#include <string>
#include <d3d12.h>
#include <map>
#include <optional>
#include <span>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// MyClass
#include "MyMath.h"
#include "TextureManager.h"

class ModelManager
{
public:
	struct VertexData {
		Float4 position;
		Float2 texcoord;
		Float3 normal;
	};

	struct MaterialData {
		std::string textureFilePath;
		uint32_t textureHandle;
	};

	struct Node {
		QuaternionTransform transform;
		Matrix localMatrix;
		std::string name;
		std::vector<Node> children;
	};

	struct VertexWeightData {
		float weight;
		uint32_t vertexIndex;
	};

	struct JointWeightData {
		Matrix inverseBindPoseMatrix;
		std::vector<VertexWeightData> vertexWeights;
	};

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
	static ModelData CreateRingModel(ID3D12Device* device);
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

