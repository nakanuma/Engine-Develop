#include "DescriptorHeap.h"
#include <cassert>

void DescriptorHeap::Create(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	heap_ = nullptr;
	type_ = heapType;
	size_ = device->GetDescriptorHandleIncrementSize(type_);
	// ディスクリプタヒープの設定を行う
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT result = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&heap_));
	assert(SUCCEEDED(result));
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandle(uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = heap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (size_ * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHandle(uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = heap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (size_ * index);
	return handleGPU;
}
