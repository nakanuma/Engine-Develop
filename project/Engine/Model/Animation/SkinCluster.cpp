#include "SkinCluster.h"

// C++
#include <cassert>

// Engine
#include <DirectXUtil.h>

void SkinCluster::CreateSkinCluster(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Skeleton& skeleton, const ModelManager::ModelData& modelData) {
	SRVManager* srvManager = SRVManager::GetInstance();

	// palette用のResourceを確保
	paletteResource_ = CreateBufferResource(device.Get(), sizeof(WellForGPU) * skeleton.joints_.size());
	WellForGPU* mappedPalette = nullptr;
	paletteResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	mappedPalette_ = {mappedPalette, skeleton.joints_.size()}; // spanを使ってアクセスするようにする
	paletteSrvHandle_.first = srvManager->GetCPUDescriptorHandle(srvManager->GetIndex());
	paletteSrvHandle_.second = srvManager->GetGPUDescriptorHandle(srvManager->GetIndex());
	SRVManager::GetInstance()->IncrementIndex(); // Indexを進める

	// palette用のsrvを作成
	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
	paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSrvDesc.Buffer.FirstElement = 0;
	paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints_.size());
	paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
	device->CreateShaderResourceView(paletteResource_.Get(), &paletteSrvDesc, paletteSrvHandle_.first);

	// influence用のResourceを確保
	influenceResource_ = CreateBufferResource(device.Get(), sizeof(VertexInfluence) * modelData.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	influenceResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size()); // 0埋め。weightを0にしておく
	mappedInfluence_ = {mappedInfluence, modelData.vertices.size()};

	// Influence用のVBVを作成
	influenceBufferView_.BufferLocation = influenceResource_->GetGPUVirtualAddress();
	influenceBufferView_.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
	influenceBufferView_.StrideInBytes = sizeof(VertexInfluence);

	// InverseBindPoseMatrixの保存領域を作成
	inverseBindPoseMatrices_.resize(skeleton.joints_.size());
	std::generate(inverseBindPoseMatrices_.begin(), inverseBindPoseMatrices_.end(), []() { return Matrix::Identity(); }); // インスタンスを作成してIdentityを呼び出す

	// ModelDataのSkinCluster情報を解析してInfluenceの中身を埋める
	for (const auto& jointWeight : modelData.skinClusterData) { // ModelのSkinClusterの情報を解析
		auto it = skeleton.jointMap_.find(jointWeight.first);   // jointWeight.firstはjoint名なので、skeletonに対象となるjointが含まれているか判断
		if (it == skeleton.jointMap_.end()) {                   // そんな名前のJointは存在しないため、次に回す
			continue;
		}
		// (*it).secondにはjointのindexが入っているので、該当のindexのinverseBindPoseMatrixを代入
		inverseBindPoseMatrices_[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			auto& currentInfluence = mappedInfluence_[vertexWeight.vertexIndex]; // 該当のvertexIndexのinfluence情報を参照しておく
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {        // 空いているところに入れる
				if (currentInfluence.weights[index] == 0.0f) {                   // weight == 0 が空いている状態なので、その場所にweightとjointのindexを代入
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}
}

void SkinCluster::Update(const Skeleton& skeleton) {
	for (size_t jointIndex = 0; jointIndex < skeleton.joints_.size(); ++jointIndex) {
		assert(jointIndex < inverseBindPoseMatrices_.size());
		mappedPalette_[jointIndex].skeletonSpaceMatrix = inverseBindPoseMatrices_[jointIndex] * skeleton.joints_[jointIndex].skeletonSpaceMatrix;
		mappedPalette_[jointIndex].skeletonSpaceInverseTransposeMatrix = Matrix::Transpose(Matrix::Inverse(mappedPalette_[jointIndex].skeletonSpaceMatrix));
	}
}
