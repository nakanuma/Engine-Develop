#include "SRVManager.h"
#include <cassert>
#include "StringUtil.h"
#include "Logger.h"

const uint32_t SRVManager::kMaxSRVCount = 128;


SRVManager* SRVManager::GetInstance()
{
	static SRVManager instance;
	return &instance;
}

void SRVManager::Initialize(DirectXBase* dxBase)
{
	// 引数で受け取ってメンバ変数に記録する
	this->dxBase = dxBase;

	// デスクリプタヒープの生成
	descriptorHeap.Create(dxBase->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount, true);
}

void SRVManager::CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = MipLevels;

	dxBase->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SRVManager::CrateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.StructureByteStride = structureByteStride;
	srvDesc.Buffer.NumElements = numElements;

	dxBase->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SRVManager::PreDraw()
{
	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap.heap_.Get() };
	dxBase->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

void SRVManager::SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex)
{
	dxBase->GetCommandList()->SetGraphicsRootDescriptorTable(RootParameterIndex, GetGPUDescriptorHandle(srvIndex));
}

D3D12_CPU_DESCRIPTOR_HANDLE SRVManager::GetCPUDescriptorHandle(uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap.GetCPUHandle(index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SRVManager::GetGPUDescriptorHandle(uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap.GetGPUHandle(index);
	return handleGPU;
}

bool SRVManager::CanAllocate()
{
	if (useIndex >= kMaxSRVCount) {
		return false;
	} else {
		return true;
	}
}

uint32_t SRVManager::Allocate()
{
	// 上限に達していないかチェックしてassert
	if (useIndex >= kMaxSRVCount) {
		assert(0);
	}

	// returnする番号を一旦記録しておく
	int index = useIndex;
	// 次回のために番号を1進める
	useIndex++;
	//　上で記録した番号をreturn
	return index;
}
