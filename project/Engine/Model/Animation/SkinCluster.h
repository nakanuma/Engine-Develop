#pragma once

// C++
#include <array>
#include <span>

// Engine
#include <DirectXBase.h>
#include <Model/Animation/Skeleton.h>
#include <ModelManager.h>
#include <MyMath.h>

/// <summary>
/// スキンクラスター（スキニング関連データの生成・更新）
/// </summary>
class SkinCluster {
public:
	// 最大4Jointの影響を受ける
	static const uint32_t kNumMaxInfluence = 4;

	struct VertexInfluence {
		std::array<float, kNumMaxInfluence> weights;
		std::array<int32_t, kNumMaxInfluence> jointIndices;
	};

	struct WellForGPU {
		Matrix skeletonSpaceMatrix;                 // 位置用
		Matrix skeletonSpaceInverseTransposeMatrix; // 法線用
	};

	/// <summary>
	/// SkinClusterの生成
	/// </summary>
	void CreateSkinCluster(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Skeleton& skeleton, const ModelManager::ModelData& modelData);

	/// <summary>
	/// SkinClusterの更新
	/// </summary>
	void Update(const Skeleton& skeleton);

	std::vector<Matrix> inverseBindPoseMatrices_;
	Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource_;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView_;
	std::span<VertexInfluence> mappedInfluence_;
	Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource_;
	std::span<WellForGPU> mappedPalette_;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle_;
};
