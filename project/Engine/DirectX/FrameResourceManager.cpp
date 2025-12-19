#include "FrameResourceManager.h"

// C++
#include <format>
#include <cassert>

// Engine
#include <MyWindow.h>
#include <Logger.h>
#include <DirectXUtil.h>

Cygnus::FrameResourceManager* Cygnus::FrameResourceManager::GetInstance() {
	static FrameResourceManager instance;
	return &instance;
}

void Cygnus::FrameResourceManager::Initialize(ID3D12Device* device, IDXGISwapChain4* swapChain)
{
	device_ = device;
	swapChain_ = swapChain;

	// レンダーターゲット生成
	CreateRenderTargets();

	// 深度ステンシルバッファ生成
	CreateDepthStencilBuffer(Window::GetWidth(), Window::GetHeight());

	// 初期化したことをログで出力
	Cygnus::Log(std::format("FrameResourceManager initialized.\n"));
}

void Cygnus::FrameResourceManager::BeginFrame(ID3D12GraphicsCommandList* commandList)
{
	// 現在のバックバッファインデックスを取得
	currentBackBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();

	// レンダーターゲット用にリソース状態を遷移
	TransitionResource(
		commandList, 
		swapChainResource_[currentBackBufferIndex_].Get(), 
		D3D12_RESOURCE_STATE_PRESENT, 
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	// レンダーターゲットと深度ステンシルビューを設定
	SetRenderTargets(commandList);

	// デフォルトのクリア処理
	ClearRenderTarget(commandList, kDefaultClearColor);
	ClearDepthStencil(commandList);
}

void Cygnus::FrameResourceManager::ClearRenderTarget(ID3D12GraphicsCommandList* commandList, const float clearColor[4])
{
	commandList->ClearRenderTargetView(rtvHandles_[currentBackBufferIndex_], clearColor, 0, nullptr);
}

void Cygnus::FrameResourceManager::ClearDepthStencil(ID3D12GraphicsCommandList* commandList, float depth)
{
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetDSVHandle();
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
}

void Cygnus::FrameResourceManager::SetRenderTargets(ID3D12GraphicsCommandList* commandList)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetCurrentRTVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetDSVHandle();

	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
}

void Cygnus::FrameResourceManager::TransitionToPresent(ID3D12GraphicsCommandList* commandList)
{
	TransitionResource(
		commandList, 
		swapChainResource_[currentBackBufferIndex_].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
		);
}

UINT Cygnus::FrameResourceManager::GetCurrentBackBufferIndex() const
{
	return currentBackBufferIndex_;
}

D3D12_CPU_DESCRIPTOR_HANDLE Cygnus::FrameResourceManager::GetCurrentRTVHandle() const
{
	return rtvHandles_[currentBackBufferIndex_];
}

ID3D12Resource* Cygnus::FrameResourceManager::GetCurrentBackBufferResource() const
{
	return swapChainResource_[currentBackBufferIndex_].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE Cygnus::FrameResourceManager::GetRTVHandle(UINT index) const
{
	// 上限を超えたインデックスが指定されたらエラー
	if(index >= kBackBufferCount){
		Log(std::format("RTV index out of Range.\n"));
		assert(0);
		return rtvHandles_[0];
	}
	return rtvHandles_[index];
}

D3D12_CPU_DESCRIPTOR_HANDLE Cygnus::FrameResourceManager::GetDSVHandle() const
{
	return dsvDescriptorHeap_.GetCPUHandle(0);
}

void Cygnus::FrameResourceManager::CreateRenderTargets()
{	
	HRESULT result = S_FALSE;

	// RTVディスクリプタヒープの生成
	rtvDescriptorHeap_.Create(device_, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kRTVHeapSize, false);

	// SwapChainからリソースを取得
	for(size_t i = 0; i < kBackBufferCount; ++i){
		result = swapChain_->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(&swapChainResource_[i]));
		assert(SUCCEEDED(result));
	}

	// RTV設定
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	// 各バックバッファ用のRTVを作成
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_.GetCPUHandle(0);
	UINT rtvDescriptorSize = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	for(size_t i = 0; i < kBackBufferCount; ++i){
		rtvHandles_[i] = rtvStartHandle;
		rtvHandles_[i].ptr += i * rtvDescriptorSize;

		device_->CreateRenderTargetView(swapChainResource_[i].Get(), &rtvDesc_, rtvHandles_[i]);
	}

	Log(std::format("Created render targets.\n"));
}

void Cygnus::FrameResourceManager::CreateDepthStencilBuffer(uint32_t width, uint32_t height)
{
	// 深度ステンシルリソース生成
	depthStencilResource_ = CreateDepthStencilTextureResource(device_, width, height, false);

	// DSVディスクリプタヒープの生成
	dsvDescriptorHeap_.Create(device_, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, kDSVHeapSize, false);

	// DSV設定
	dsvDesc_.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc_.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	// DSVを作成
	device_->CreateDepthStencilView(
		depthStencilResource_.Get(),
		&dsvDesc_,
		dsvDescriptorHeap_.GetCPUHandle(0)
	);

	Log(std::format("Created depth stencil buffer.\n"));
}

void Cygnus::FrameResourceManager::TransitionResource(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
{
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier_.Transition.pResource = resource;
	barrier_.Transition.StateBefore = stateBefore;
	barrier_.Transition.StateAfter = stateAfter;
	barrier_.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &barrier_);
}