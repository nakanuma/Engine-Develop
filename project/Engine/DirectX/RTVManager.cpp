#include "RTVManager.h"
#include "DirectXBase.h"
#include "DirectXUtil.h"
#include "TextureManager.h"

Cygnus::RTVManager& Cygnus::RTVManager::GetInstance() {
	static RTVManager instance;

	return instance;
}

int32_t Cygnus::RTVManager::CreateRenderTargetTexture(uint32_t width, uint32_t height, const Float4& clearColor) {
	// 空のテクスチャを作成
	int32_t emptyTexture = TextureManager::CreateEmptyTexture(width, height, clearColor);
	// TextureHandleとRTVHandleを対応させる
	GetInstance().rtvHandleMap_[emptyTexture] = GetInstance().rtvIndex_;

	// テクスチャに対してレンダーターゲットを作成
	DirectXBase::GetInstance()->GetDevice()->CreateRenderTargetView(TextureManager::GetResource(emptyTexture), nullptr, DirectXBase::GetInstance()->GetRTVHeap()->GetCPUHandle(GetInstance().rtvIndex_));

	// 深度テクスチャの作成
	// DepthStencilTextureをウィンドウのサイズで作成
	ID3D12Resource* depthResource;
	GetInstance().dsvResourceMap_[emptyTexture] = CreateDepthStencilTextureResource(DirectXBase::GetInstance()->GetDevice(), Window::GetWidth(), Window::GetHeight(), true);

	depthResource = GetInstance().dsvResourceMap_[emptyTexture].Get();

	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};

	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;        // Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2dTexture;
	// DSVHeapの先頭にDSVをつくる
	DirectXBase::GetInstance()->GetDevice()->CreateDepthStencilView(depthResource, &dsvDesc, DirectXBase::GetInstance()->GetDSVHeap()->GetCPUHandle(GetInstance().rtvIndex_)); // rtvIndexと同じにする

	// SRVの設定をする
	uint32_t depthSRVHandle = TextureManager::CreateSRV(depthResource, DXGI_FORMAT_R24_UNORM_X8_TYPELESS);
	GetInstance().depthSRVHandleMap_[emptyTexture] = depthSRVHandle;

	GetInstance().rtvIndex_++;

	return emptyTexture;
}

void Cygnus::RTVManager::SetRenderTarget(int32_t textureHandle) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 元のレンダーターゲットのリソースバリアを戻す
	ResetResourceBarrier();

	// リソースバリアを書き込み可能な状態にする
	dxBase->barrier_.Transition.pResource = TextureManager::GetResource(textureHandle);
	dxBase->barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	dxBase->barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	dxBase->GetCommandList()->ResourceBarrier(1, &dxBase->barrier_);
	// そのテクスチャの深度情報のリソースバリアを書込み可能な状態にする
	dxBase->barrier_.Transition.pResource = GetInstance().dsvResourceMap_[textureHandle].Get();
	dxBase->barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
	dxBase->barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	dxBase->GetCommandList()->ResourceBarrier(1, &dxBase->barrier_);

	// レンダーターゲットをセットする
	auto cpuHandle = dxBase->GetRTVHeap()->GetCPUHandle(GetInstance().rtvHandleMap_[textureHandle]);
	auto dsvHandle = dxBase->dsvDescriptorHeap_.GetCPUHandle(GetInstance().rtvHandleMap_[textureHandle]);

	dxBase->GetCommandList()->OMSetRenderTargets(1, &cpuHandle, false, &dsvHandle);

	// 現在のレンダーターゲットを保存する
	GetInstance().currentRenderTarget_ = textureHandle;
}

void Cygnus::RTVManager::SetRenderTarget(int32_t textureHandle, int32_t depthSourceHandle)
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 元のレンダーターゲットのリソースバリアを戻す
	ResetResourceBarrier();

	// リソースバリアを書き込み可能な状態にする
	dxBase->barrier_.Transition.pResource = TextureManager::GetResource(textureHandle);
	dxBase->barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	dxBase->barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	dxBase->GetCommandList()->ResourceBarrier(1, &dxBase->barrier_);
	// そのテクスチャの深度情報のリソースバリアを書込み可能な状態にする
	dxBase->barrier_.Transition.pResource = GetInstance().dsvResourceMap_[depthSourceHandle].Get();
	dxBase->barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
	dxBase->barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	dxBase->GetCommandList()->ResourceBarrier(1, &dxBase->barrier_);

	// レンダーターゲットをセットする
	auto cpuHandle = dxBase->GetRTVHeap()->GetCPUHandle(GetInstance().rtvHandleMap_[textureHandle]);
	auto dsvHandle = dxBase->dsvDescriptorHeap_.GetCPUHandle(GetInstance().rtvHandleMap_[depthSourceHandle]);

	dxBase->GetCommandList()->OMSetRenderTargets(1, &cpuHandle, false, &dsvHandle);

	// 現在のレンダーターゲットを保存する
	GetInstance().currentRenderTarget_ = textureHandle;
}

void Cygnus::RTVManager::SetRTtoBB() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 元のレンダーターゲットのリソースバリアを戻す
	ResetResourceBarrier();

	// リソースバリアを書き込み可能な状態にする

	UINT backBufferIndex = dxBase->swapChain_->GetCurrentBackBufferIndex();

	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	dxBase->barrier_.Transition.pResource = dxBase->swapChainResources_[backBufferIndex].Get();
	// 遷移前（現在）のResourceState
	dxBase->barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	// 遷移後のResourceState
	dxBase->barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// TransitionBarrierを張る
	dxBase->GetCommandList()->ResourceBarrier(1, &dxBase->barrier_);

	// 描画先のRTVをとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxBase->dsvDescriptorHeap_.GetCPUHandle(0);

	// レンダーターゲットをセットする
	dxBase->GetCommandList()->OMSetRenderTargets(1, &dxBase->rtvHandles_[backBufferIndex], false, &dsvHandle);

	// 現在のレンダーターゲットを保存する
	GetInstance().currentRenderTarget_ = -1;
}

void Cygnus::RTVManager::ResetResourceBarrier() {
	int32_t rt = GetInstance().currentRenderTarget_;
	DirectXBase* dxBase = DirectXBase::GetInstance();

	if (rt < 0) {
		// バックバッファのリソースバリアを戻す
		UINT backBufferIndex = dxBase->swapChain_->GetCurrentBackBufferIndex();

		dxBase->barrier_.Transition.pResource = dxBase->swapChainResources_[backBufferIndex].Get();
		dxBase->barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		dxBase->barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		dxBase->GetCommandList()->ResourceBarrier(1, &dxBase->barrier_);
	} else {
		// レンダーテクスチャのリソースバリアを戻す
		dxBase->barrier_.Transition.pResource = TextureManager::GetResource(rt);
		dxBase->barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		dxBase->barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		dxBase->GetCommandList()->ResourceBarrier(1, &dxBase->barrier_);
		// そのテクスチャの深度情報のリソースバリアを戻す
		dxBase->barrier_.Transition.pResource = GetInstance().dsvResourceMap_[rt].Get();
		dxBase->barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		dxBase->barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		dxBase->GetCommandList()->ResourceBarrier(1, &dxBase->barrier_);
	}
}

void Cygnus::RTVManager::ClearRTV(int32_t textureHandle, const Float4& clearColor) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 指定した色で画面全体をクリアする
	dxBase->GetCommandList()->ClearRenderTargetView(dxBase->GetRTVHeap()->GetCPUHandle(GetInstance().rtvHandleMap_[textureHandle]), &clearColor.x, 0, nullptr);
	dxBase->GetCommandList()->ClearDepthStencilView(dxBase->GetDSVHeap()->GetCPUHandle(GetInstance().rtvHandleMap_[textureHandle]), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

int32_t Cygnus::RTVManager::GetDepthSRVHandle(int32_t textureHandle) { return GetInstance().depthSRVHandleMap_[textureHandle]; }

D3D12_CPU_DESCRIPTOR_HANDLE Cygnus::RTVManager::GetDSVHandle(uint32_t textureHandle)
{
	DirectXBase* dxBase = DirectXBase::GetInstance();
	
	auto it = GetInstance().rtvHandleMap_.find(textureHandle);

	if(it != GetInstance().rtvHandleMap_.end()){
		UINT dsvIndex = it->second;
		return dxBase->GetDSVHeap()->GetCPUHandle(dsvIndex);
	}

	return {0};
}

ID3D12Resource* Cygnus::RTVManager::GetDepthResource(uint32_t textureHandle)
{
	auto it = GetInstance().dsvResourceMap_.find(textureHandle);

	if(it != GetInstance().dsvResourceMap_.end()){
		return it->second.Get();
	}

	return nullptr;
}
