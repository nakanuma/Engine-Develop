#include "SRVManager.h"
#include "Logger.h"
#include "StringUtil.h"
#include <cassert>

const uint32_t Cygnus::SRVManager::kMaxSRVCount = 16384;

Cygnus::SRVManager* Cygnus::SRVManager::GetInstance() {
	static SRVManager instance;
	return &instance;
}

void Cygnus::SRVManager::Initialize(DirectXBase* dxBase) {
	// 引数で受け取ってメンバ変数に記録する
	this->dxBase_ = dxBase;

	// デスクリプタヒープの生成
	descriptorHeap_.Create(dxBase->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount, true);
}

void Cygnus::SRVManager::CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels) {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = Format; // フォーマット
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; // 色成分マッピングはデフォルトに
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = MipLevels; // ミップマップのレベル

	// SRVを生成して指定されたヒープの位置に登録
	dxBase_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void Cygnus::SRVManager::CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride) {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // フォーマットは未指定
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER; // ビューの次元をバッファとして指定
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; // マッピング設定
	srvDesc.Buffer.StructureByteStride = structureByteStride; // 1要素のサイズ
	srvDesc.Buffer.NumElements = numElements; // バッファ内要素数

	// SRVを生成して指定されたヒープの位置に登録
	dxBase_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void Cygnus::SRVManager::PreDraw() {
	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = {descriptorHeap_.heap_.Get()};
	dxBase_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

void Cygnus::SRVManager::SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex) {
	dxBase_->GetCommandList()->SetGraphicsRootDescriptorTable(RootParameterIndex, GetGPUDescriptorHandle(srvIndex));
}

D3D12_CPU_DESCRIPTOR_HANDLE Cygnus::SRVManager::GetCPUDescriptorHandle(uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_.GetCPUHandle(index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE Cygnus::SRVManager::GetGPUDescriptorHandle(uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_.GetGPUHandle(index);
	return handleGPU;
}

bool Cygnus::SRVManager::CanAllocate() {
	return useIndex_ < kMaxSRVCount;
}

uint32_t Cygnus::SRVManager::Allocate() {
	// 上限に達していないかチェックしてassert
	if (useIndex_ >= kMaxSRVCount) {
		assert(0);
	}

	// returnする番号を一旦記録しておく
	int index = useIndex_;
	// 次回のために番号を1進める
	useIndex_++;
	// 　上で記録した番号をreturn
	return index;
}
