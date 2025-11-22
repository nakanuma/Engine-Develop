#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <array>
#include <span>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <DirectXBase.h>
#include <Model/Animation/Skeleton.h>
#include <ModelManager.h>
#include <MyMath.h>

// =========================================================
// スキンクラスター（スキニング関連データの生成・更新）
// =========================================================
class SkinCluster {
public:
	// 最大4Jointの影響を受ける
	static constexpr uint32_t kNumMaxInfluence = 4;

	/// <summary>
	/// 頂点が受けるジョイントの影響情報
	/// </summary>
	struct VertexInfluence {
		std::array<float, kNumMaxInfluence> weights;			/* 各ジョイントの影響度 */
		std::array<int32_t, kNumMaxInfluence> jointIndices;		/* 各ジョイントのインデックス */
	};

	/// <summary>
	/// GPUに渡すスケルトン空間での変換行列
	/// </summary>
	struct WellForGPU {
		Matrix skeletonSpaceMatrix;								/* 4x4変換行列 */
		Matrix skeletonSpaceInverseTransposeMatrix;				/* 4x4逆転置行列 */
	};

	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// SkinClusterの生成を行います。
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="skeleton">スケルトン</param>
	/// <param name="modelData">モデルデータ</param>
	void CreateSkinCluster(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Skeleton& skeleton, const ModelManager::ModelData& modelData);

	/// <summary>
	/// SkinClusterの毎フレーム更新処理を行います。
	/// </summary>
	/// <param name="skeleton">スケルトン</param>
	void Update(const Skeleton& skeleton);

	// =========================================================
	// Member Variables
	// =========================================================

	std::vector<Matrix> inverseBindPoseMatrices_;											/* 各ジョイントのバインドポーズ逆行列 */
	Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource_;								/* 頂点のジョイント影響情報リソース */
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView_;											/* 頂点バッファビュー */
	std::span<VertexInfluence> mappedInfluence_;											/* マッピング後の頂点影響情報配列 */
	Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource_;								/* ジョイント変換行列リソース */
	std::span<WellForGPU> mappedPalette_;													/* マッピング後のジョイント変換行列配列 */
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle_;	/* ジョイント変換行列SRVハンドル */
};
