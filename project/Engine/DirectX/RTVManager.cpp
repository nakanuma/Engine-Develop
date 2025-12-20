#include "RTVManager.h"

// Engine
#include <DirectXBase.h>
#include <DirectXUtil.h>
#include <TextureManager.h>
#include <FrameResourceManager.h>
#include <CommandManager.h>

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
	DirectXBase::GetInstance()->GetDevice()->CreateRenderTargetView(
		TextureManager::GetResource(emptyTexture), 
		nullptr, 
		FrameResourceManager::GetInstance()->GetRTVHeap()->GetCPUHandle(GetInstance().rtvIndex_)
	);

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
	DirectXBase::GetInstance()->GetDevice()->CreateDepthStencilView(
		depthResource, 
		&dsvDesc, 
		FrameResourceManager::GetInstance()->GetDSVHeap()->GetCPUHandle(GetInstance().rtvIndex_) // rtvIndexと同じにする
	);

	// SRVの設定をする
	uint32_t depthSRVHandle = TextureManager::CreateSRV(depthResource, DXGI_FORMAT_R24_UNORM_X8_TYPELESS);
	GetInstance().depthSRVHandleMap_[emptyTexture] = depthSRVHandle;

	GetInstance().rtvIndex_++;

	return emptyTexture;
}

void Cygnus::RTVManager::SetRenderTarget(int32_t textureHandle) {
	auto cmd = CommandManager::GetInstance()->GetCommandList();
	FrameResourceManager* frameResource = FrameResourceManager::GetInstance();

	// 元のレンダーターゲットのリソースバリアを戻す
	ResetResourceBarrier();

	// リソースバリアを書き込み可能な状態にする
	TransitionResource(
		cmd,
		TextureManager::GetResource(textureHandle),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	// そのテクスチャの深度情報のリソースバリアを書込み可能な状態にする
	TransitionResource(
		cmd,
		GetInstance().dsvResourceMap_[textureHandle].Get(), 
		D3D12_RESOURCE_STATE_GENERIC_READ, 
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);

	// レンダーターゲットをセットする
	auto cpuHandle = frameResource->GetRTVHeap()->GetCPUHandle(GetInstance().rtvHandleMap_[textureHandle]);
	auto dsvHandle = frameResource->GetDSVHeap()->GetCPUHandle(GetInstance().rtvHandleMap_[textureHandle]);

	cmd->OMSetRenderTargets(1, &cpuHandle, false, &dsvHandle);

	// 現在のレンダーターゲットを保存する
	GetInstance().currentRenderTarget_ = textureHandle;
}

void Cygnus::RTVManager::SetRenderTarget(int32_t textureHandle, int32_t depthSourceHandle)
{
	auto cmd = CommandManager::GetInstance()->GetCommandList();
	FrameResourceManager* frameResource = FrameResourceManager::GetInstance();

	// 元のレンダーターゲットのリソースバリアを戻す
	ResetResourceBarrier();

	// リソースバリアを書き込み可能な状態にする
	TransitionResource(
		cmd,
		TextureManager::GetResource(textureHandle),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	// そのテクスチャの深度情報のリソースバリアを書込み可能な状態にする
	TransitionResource(
		cmd,
		GetInstance().dsvResourceMap_[textureHandle].Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);

	// レンダーターゲットをセットする
	auto cpuHandle = frameResource->GetRTVHeap()->GetCPUHandle(GetInstance().rtvHandleMap_[textureHandle]);
	auto dsvHandle = frameResource->GetDSVHeap()->GetCPUHandle(GetInstance().rtvHandleMap_[depthSourceHandle]);

	cmd->OMSetRenderTargets(1, &cpuHandle, false, &dsvHandle);

	// 現在のレンダーターゲットを保存する
	GetInstance().currentRenderTarget_ = textureHandle;
}

void Cygnus::RTVManager::SetRTtoBB() {
	auto cmd = CommandManager::GetInstance()->GetCommandList();
	FrameResourceManager* frameResource = FrameResourceManager::GetInstance();

	// 元のレンダーターゲットのリソースバリアを戻す
	ResetResourceBarrier();

	// リソースバリアを書き込み可能な状態にする
	TransitionResource(
		cmd,
		frameResource->GetCurrentBackBufferResource(), 
		D3D12_RESOURCE_STATE_PRESENT, 
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = frameResource->GetCurrentRTVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = frameResource->GetDSVHeap()->GetCPUHandle(0);

	// レンダーターゲットをセットする
	cmd->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	// 現在のレンダーターゲットを保存する
	GetInstance().currentRenderTarget_ = -1;
}

void Cygnus::RTVManager::ResetResourceBarrier() {
	int32_t rt = GetInstance().currentRenderTarget_;
	auto cmd = CommandManager::GetInstance()->GetCommandList();
	FrameResourceManager* frameResource = FrameResourceManager::GetInstance();

	if (rt < 0) {
		// バックバッファのリソースバリアを戻す
		TransitionResource(
			cmd,
			frameResource->GetCurrentBackBufferResource(), 
			D3D12_RESOURCE_STATE_RENDER_TARGET, 
			D3D12_RESOURCE_STATE_PRESENT
		);

	} else {
		// レンダーテクスチャのリソースバリアを戻す
		TransitionResource(
			cmd,
			TextureManager::GetResource(rt), 
			D3D12_RESOURCE_STATE_RENDER_TARGET, 
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

		// そのテクスチャの深度情報のリソースバリアを戻す
		TransitionResource(
			cmd,
			GetInstance().dsvResourceMap_[rt].Get(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			D3D12_RESOURCE_STATE_GENERIC_READ
			);
	}
}

void Cygnus::RTVManager::ClearRTV(int32_t textureHandle, const Float4& clearColor) {
	auto cmd = CommandManager::GetInstance()->GetCommandList();
	FrameResourceManager* frameResource = FrameResourceManager::GetInstance();

	// 指定した色で画面全体をクリアする
	cmd->ClearRenderTargetView(frameResource->GetRTVHeap()->GetCPUHandle(GetInstance().rtvHandleMap_[textureHandle]), &clearColor.x, 0, nullptr);
	cmd->ClearDepthStencilView(frameResource->GetDSVHeap()->GetCPUHandle(GetInstance().rtvHandleMap_[textureHandle]), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

int32_t Cygnus::RTVManager::GetDepthSRVHandle(int32_t textureHandle) { return GetInstance().depthSRVHandleMap_[textureHandle]; }

D3D12_CPU_DESCRIPTOR_HANDLE Cygnus::RTVManager::GetDSVHandle(uint32_t textureHandle)
{
	FrameResourceManager* frameResource = FrameResourceManager::GetInstance();
	
	auto it = GetInstance().rtvHandleMap_.find(textureHandle);

	if(it != GetInstance().rtvHandleMap_.end()){
		UINT dsvIndex = it->second;
		return frameResource->GetDSVHeap()->GetCPUHandle(dsvIndex);
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
