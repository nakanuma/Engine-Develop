#pragma once

// ---------------------------------------------------------
// Externals Includes
// ---------------------------------------------------------
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <d3d12.h>
#include <map>
#include <optional>
#include <span>
#include <string>
#include <vector>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <MyMath.h>
#include <TextureManager.h>

// =========================================================
// モデル管理クラス
// =========================================================
class ModelManager {
public:
	/// <summary>
	/// 頂点データ構造体
	/// </summary>
	struct VertexData {
		Float4 position;															/* 頂点位置 */
		Float2 texcoord;															/* テクスチャ座標 */
		Float3 normal;																/* 法線ベクトル */
	};

	/// <summary>
	/// マテリアル情報
	/// </summary>
	struct MaterialData {
		std::string textureFilePath;												/* テクスチャファイルパス */
		uint32_t textureHandle;														/* テクスチャハンドル */
	};

	/// <summary>
	/// ノード構造体
	/// </summary>
	struct Node {
		QuaternionTransform transform;												/* ローカル変換情報 */
		Matrix localMatrix;															/* ローカル変換行列 */
		std::string name;															/* ノード名 */
		std::vector<Node> children;													/* 子ノード配列 */
	};

	/// <summary>
	/// 頂点に対するジョイントの影響情報
	/// </summary>
	struct VertexWeightData {
		float weight;																/* ジョイントの影響度合い */
		uint32_t vertexIndex;														/* 頂点インデックス */
	};

	/// <summary>
	/// ジョイントごとのスキニング情報
	/// </summary>
	struct JointWeightData {
		Matrix inverseBindPoseMatrix;												/* 逆バインドポーズ行列 */
		std::vector<VertexWeightData> vertexWeights;								/* 頂点ごとの影響情報配列 */
	};

	/// <summary>
	/// モデル全体のデータ構造
	/// </summary>
	struct ModelData {
		std::map<std::string, JointWeightData> skinClusterData;						/* スキンクラスター情報マップ */
		std::vector<VertexData> vertices;											/* 頂点データ配列 */
		std::vector<uint32_t> indices;												/* インデックスデータ配列 */
		MaterialData material;														/* マテリアルデータ */
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;						/* 頂点バッファリソース */
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;									/* 頂点バッファビュー */
		Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;						/* インデックスバッファリソース */
		D3D12_INDEX_BUFFER_VIEW indexBufferView;									/* インデックスバッファビュー */
		Node rootNode;																/* ルートノード */
	};

	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// リングモデルの生成を行います。
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="outerRadius">外半径</param>
	/// <param name="innerRadius">内半径</param>
	/// <returns>リングモデル</returns>
	static ModelData CreateRingModel(ID3D12Device* device, float outerRadius = 1.0f, float innerRadius = 0.2f);

	/// <summary>
	/// シリンダーモデルの生成を行います。
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <returns>シリンダーモデル</returns>
	static ModelData CreateCylinderModel(ID3D12Device* device);
	
	/// <summary>
	/// スカイボックスモデルの生成を行います。
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <returns>スカイボックスモデル</returns>
	static ModelData CreateSkyBoxModel(ID3D12Device* device);



	/// <summary>
	/// モデルの読み込みを行います。
	/// </summary>
	/// <param name="directoryPath">ディレクトリパス</param>
	/// <param name="filename">ファイル名</param>
	/// <param name="device">デバイス</param>
	/// <returns>モデルデータ</returns>
	static ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename, ID3D12Device* device);
	
	/// <summary>
	/// マテリアルテンプレートファイルの読み込みを行います。
	/// </summary>
	/// <param name="directoryPath">ディレクトリパス</param>
	/// <param name="filename">ファイル名</param>
	/// <param name="device">デバイス</param>
	/// <returns>マテリアルデータ</returns>
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename, ID3D12Device* device);
	
	/// <summary>
	/// AssimpのNodeから、Node構造体に変換します。
	/// </summary>
	/// <param name="node">AssimpのNode</param>
	/// <returns>Node構造体</returns>
	static Node ReadNode(aiNode* node);
};
