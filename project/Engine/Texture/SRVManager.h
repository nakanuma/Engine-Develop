#pragma once
#include "DirectXBase.h"
#include "DescriptorHeap.h"

// SRV管理
class SRVManager
{
public:
	static SRVManager* GetInstance();

	// 初期化
	void Initialize(DirectXBase* dxBase);
	// SRV生成（テクスチャ用）
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	// SRV生成（Structured Buffer用）
	void CrateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

	uint32_t Allocate();
	void PreDraw();
	void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
	const uint32_t GetIndex() { return useIndex; }
	void IncrementIndex() { useIndex++; }

	bool CanAllocate();

	// SRV用デスクリプタヒープ
	DescriptorHeap descriptorHeap;
private:
	DirectXBase* dxBase = nullptr;

	// 最大SRV数（最大テクスチャ枚数）
	static const uint32_t kMaxSRVCount;
	// SRV用のデスクリプタサイズ
	uint32_t descriptorSize;
	// 次に使用するSRVインデックス
	uint32_t useIndex = 1;
};

