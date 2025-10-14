#pragma once
#include <d3d12.h>
#include <stdint.h>
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")

/// <summary>
/// ディスクリプタヒープのラッパークラス
/// </summary>
class DescriptorHeap {
public:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_;

	void Create(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t index);

private:
	D3D12_DESCRIPTOR_HEAP_TYPE type_;
	uint32_t size_;
};
