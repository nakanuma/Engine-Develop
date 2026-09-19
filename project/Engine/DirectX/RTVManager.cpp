#include "RTVManager.h"

// Engine
#include <DirectXBase.h>
#include <DirectXUtil.h>
#include <TextureManager.h>
#include <FrameResourceManager.h>
#include <CommandManager.h>
#include <Logger.h>

Cygnus::RTVManager& Cygnus::RTVManager::GetInstance() {
	static RTVManager instance;

	return instance;
}

int32_t Cygnus::RTVManager::CreateRenderTargetTexture(uint32_t width, uint32_t height, const Float4& clearColor, DXGI_FORMAT format) {
	// 空のテクスチャを作成
	int32_t emptyTexture = TextureManager::CreateEmptyTexture(width, height, clearColor, format);
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

	// DSVを作成する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = FrameResourceManager::GetInstance()->GetDSVHeap()->GetCPUHandle(GetInstance().rtvIndex_);
	DirectXBase::GetInstance()->GetDevice()->CreateDepthStencilView(
		depthResource,
		&dsvDesc,
		dsvHandle
	);
	// textureHandleとDSVHandleを対応
	GetInstance().dsvHandleMap_[emptyTexture] = dsvHandle;

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

	// カラーリソース
	TransitionResource(
		cmd,
		TextureManager::GetResource(textureHandle),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	// 深度リソース
	TransitionResource(
		cmd,
		GetInstance().dsvResourceMap_[textureHandle].Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);

	// レンダーターゲットをセットする
	auto rtvHandle = frameResource->GetRTVHeap()->GetCPUHandle(GetInstance().rtvHandleMap_[textureHandle]);
	auto dsvHandle = GetInstance().dsvHandleMap_.at(textureHandle);

	cmd->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

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
		GetInstance().dsvResourceMap_[depthSourceHandle].Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);

	// レンダーターゲットをセットする
	auto cpuHandle = frameResource->GetRTVHeap()->GetCPUHandle(GetInstance().rtvHandleMap_[textureHandle]);
	auto dsvHandle = GetInstance().dsvHandleMap_.at(depthSourceHandle);

	cmd->OMSetRenderTargets(1, &cpuHandle, false, &dsvHandle);

	// 現在のレンダーターゲットを保存する
	GetInstance().currentRenderTarget_ = textureHandle;
}

void Cygnus::RTVManager::SetRTtoBB() {
	auto cmd = CommandManager::GetInstance()->GetCommandList();
	FrameResourceManager* frameResource = FrameResourceManager::GetInstance();

	// 現在のオフスクリーンRTを終了
	ResetResourceBarrier();

	// BackBufferを描画可能にする
	/*TransitionResource(
		cmd,
		frameResource->GetCurrentBackBufferResource(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);*/

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

	if (rt < 0) { return; }

	// オフスクリーンRT
	TransitionResource(
		cmd,
		TextureManager::GetResource(rt),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);

	// Depth
	TransitionResource(
		cmd,
		GetInstance().dsvResourceMap_[rt].Get(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
}

void Cygnus::RTVManager::ClearRTV(int32_t textureHandle, const Float4& clearColor) {
	auto cmd = CommandManager::GetInstance()->GetCommandList();
	FrameResourceManager* frameResource = FrameResourceManager::GetInstance();

	// 指定した色で画面全体をクリアする
	cmd->ClearRenderTargetView(frameResource->GetRTVHeap()->GetCPUHandle(GetInstance().rtvHandleMap_[textureHandle]), &clearColor.x, 0, nullptr);
	cmd->ClearDepthStencilView(frameResource->GetDSVHeap()->GetCPUHandle(GetInstance().rtvHandleMap_[textureHandle]), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void Cygnus::RTVManager::ClearDepth(int32_t textureHandle)
{
	auto cmd = CommandManager::GetInstance()->GetCommandList();
	FrameResourceManager* frameResource = FrameResourceManager::GetInstance();

	cmd->ClearDepthStencilView(
		GetInstance().dsvHandleMap_.at(textureHandle),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0,
		0,
		nullptr
	);
}

int32_t Cygnus::RTVManager::GetDepthSRVHandle(int32_t textureHandle) { return GetInstance().depthSRVHandleMap_[textureHandle]; }

D3D12_CPU_DESCRIPTOR_HANDLE Cygnus::RTVManager::GetDSVHandle(uint32_t textureHandle)
{
	FrameResourceManager* frameResource = FrameResourceManager::GetInstance();

	auto it = GetInstance().rtvHandleMap_.find(textureHandle);

	if (it != GetInstance().rtvHandleMap_.end()) {
		UINT dsvIndex = it->second;
		return frameResource->GetDSVHeap()->GetCPUHandle(dsvIndex);
	}

	return { 0 };
}

ID3D12Resource* Cygnus::RTVManager::GetDepthResource(uint32_t textureHandle)
{
	auto it = GetInstance().dsvResourceMap_.find(textureHandle);

	if (it != GetInstance().dsvResourceMap_.end()) {
		return it->second.Get();
	}

	return nullptr;
}

void Cygnus::RTVManager::SetDepthOnlyRenderTarget(int32_t textureHandle)
{
	auto* cmd = CommandManager::GetInstance()->GetCommandList();

	ResetResourceBarrier();

	ID3D12Resource* depthResource = GetInstance().dsvResourceMap_.at(textureHandle).Get();

	TransitionResource(
		cmd,
		depthResource,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetInstance().dsvHandleMap_.at(textureHandle);

	cmd->OMSetRenderTargets(0, nullptr, FALSE, &dsvHandle);
}

void Cygnus::RTVManager::TransitionDepthToShaderResource(int32_t textureHandle)
{
	auto* cmd = CommandManager::GetInstance()->GetCommandList();

	TransitionResource(
		cmd,
		GetInstance().dsvResourceMap_[textureHandle].Get(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
}

void Cygnus::RTVManager::SetRenderTargetKeepDepth(int32_t textureHandle)
{
	auto cmd = CommandManager::GetInstance()->GetCommandList();
	auto* frameResource = FrameResourceManager::GetInstance();

	ResetResourceBarrier();

	// カラーRT
	TransitionResource(
		cmd,
		TextureManager::GetResource(textureHandle),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	auto rtvHandle = frameResource->GetRTVHeap()->GetCPUHandle(GetInstance().rtvHandleMap_[textureHandle]);
	auto dsvHandle = GetInstance().dsvHandleMap_.at(textureHandle);

	cmd->OMSetRenderTargets(
		1,
		&rtvHandle,
		FALSE,
		&dsvHandle
	);

	GetInstance().currentRenderTarget_ = textureHandle;
}

void Cygnus::RTVManager::SetCubeMapRenderTarget(int32_t textureHandle, uint32_t mipLevel, uint32_t face)
{
	assert(face < 6);

	ID3D12Resource* resource = TextureManager::GetInstance().GetResource(textureHandle);

	assert(resource != nullptr);

	const DirectX::TexMetadata& metadata = TextureManager::GetInstance().GetMetaData(textureHandle);

	assert(metadata.IsCubemap());
	assert(mipLevel < metadata.mipLevels);

	/* Face × Mip を識別するキー */
	int64_t key = 
		(static_cast<int64_t>(textureHandle) << 32) | 
		(static_cast<int64_t>(mipLevel) << 8) |
		static_cast<int64_t>(face);

	/* RTVがまだ存在しなければ作成 */
	auto it = GetInstance().cubeMapRTVHandleMap_.find(key);

	if(it == GetInstance().cubeMapRTVHandleMap_.end()) {
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = FrameResourceManager::GetInstance()->GetRTVHeap()->GetCPUHandle(GetInstance().rtvIndex_);

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

		rtvDesc.Format = metadata.format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;

		rtvDesc.Texture2DArray.MipSlice = mipLevel;
		rtvDesc.Texture2DArray.FirstArraySlice = face;
		rtvDesc.Texture2DArray.ArraySize = 1;
		rtvDesc.Texture2DArray.PlaneSlice = 0;

		DirectXBase::GetInstance()->GetDevice()->CreateRenderTargetView(
			resource, 
			&rtvDesc, 
			rtvHandle
		);

		GetInstance().cubeMapRTVHandleMap_[key] = rtvHandle;

		GetInstance().rtvIndex_++;

		it = GetInstance().cubeMapRTVHandleMap_.find(key);
	}

	// Face × Mip のサブリソース番号
	uint32_t subresource = mipLevel + face * static_cast<uint32_t>(metadata.mipLevels);

	/* CubeMap全体をRT状態へ */
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource;
	barrier.Transition.Subresource = subresource;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	CommandManager::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);

	/* レンダーターゲット設定 */
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = it->second;

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetInstance().cubeMapDSVHandleMap_.at(textureHandle);

	ID3D12Resource* depthResource = GetInstance().cubeMapDepthResourceMap_.at(textureHandle).Get();

	TransitionResource(
		CommandManager::GetInstance()->GetCommandList(),
		depthResource,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);

	CommandManager::GetInstance()->GetCommandList()->OMSetRenderTargets(
		1, 
		&rtvHandle, 
		FALSE, 
		&dsvHandle
	);

	/*GetInstance().currentRenderTarget_ = textureHandle;*/

	Log(std::format(
		"SetCubeMapRenderTarget: handle={}, mip={}, face={}, subresource={}\n",
		textureHandle,
		mipLevel,
		face,
		subresource
	));
}

void Cygnus::RTVManager::ResetCubeMapResourceBarrier(int32_t textureHandle, uint32_t mipLevel, uint32_t face)
{
	assert(face < 6);

	ID3D12Resource* resource = TextureManager::GetInstance().GetResource(textureHandle);

	assert(resource != nullptr);

	const DirectX::TexMetadata& metadata = TextureManager::GetInstance().GetMetaData(textureHandle);

	assert(metadata.IsCubemap());

	// Face × Mip のサブリソース番号
	uint32_t subresource = mipLevel + face * static_cast<uint32_t>(metadata.mipLevels);

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource;
	barrier.Transition.Subresource = subresource;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	CommandManager::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);

	ID3D12Resource* depthResource = GetInstance().cubeMapDepthResourceMap_.at(textureHandle).Get();

	TransitionResource(
		CommandManager::GetInstance()->GetCommandList(),
		depthResource,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);

	Log(std::format(
		"ResetCubeMapResourceBarrier: handle={}, mip={}, face={}, subresource={}\n",
		textureHandle,
		mipLevel,
		face,
		subresource
	));
}

void Cygnus::RTVManager::ClearCubeMapRTV(int32_t textureHandle, uint32_t mipLevel, uint32_t face, const Float4& clearColor)
{
	assert(face < 6);

	ID3D12Resource* resource = TextureManager::GetInstance().GetResource(textureHandle);

	assert(resource != nullptr);

	const DirectX::TexMetadata& metadata = TextureManager::GetInstance().GetMetaData(textureHandle);

	assert(metadata.IsCubemap());
	assert(mipLevel < metadata.mipLevels);

	/* Face × Mip を識別するキー */
	int64_t key =
		(static_cast<int64_t>(textureHandle) << 32) |
		(static_cast<int64_t>(mipLevel) << 8) |
		static_cast<int64_t>(face);

	auto it = GetInstance().cubeMapRTVHandleMap_.find(key);

	assert(it != GetInstance().cubeMapRTVHandleMap_.end());

	FLOAT color[] = {
		clearColor.x,
		clearColor.y,
		clearColor.z,
		clearColor.w
	};

	CommandManager::GetInstance()->GetCommandList()->ClearRenderTargetView(
		it->second,
		color,
		0,
		nullptr
	);
}

void Cygnus::RTVManager::CreateCubeMapDepth(int32_t textureHandle, uint32_t width, uint32_t height)
{
	assert(textureHandle >= 0);
	assert(width > 0);
	assert(height > 0);

	// CubeMap専用DepthResource
	GetInstance().cubeMapDepthResourceMap_[textureHandle] = 
		CreateDepthStencilTextureResource(DirectXBase::GetInstance()->GetDevice(), width, height, true);

	ID3D12Resource* depthResource = GetInstance().cubeMapDepthResourceMap_[textureHandle].Get();

	assert(depthResource != nullptr);

	// DSV descriptor
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	// DSV heapから確保
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = FrameResourceManager::GetInstance()->GetDSVHeap()->GetCPUHandle(GetInstance().rtvIndex_);

	DirectXBase::GetInstance()->GetDevice()->CreateDepthStencilView(depthResource, &dsvDesc, dsvHandle);

	GetInstance().cubeMapDSVHandleMap_[textureHandle] = dsvHandle;

	Log(std::format(
		"CreateCubeMapDepth: handle={}, width={}, height={}\n",
		textureHandle,
		width,
		height
	));
}

void Cygnus::RTVManager::ClearRenderTargetOnly(int32_t textureHandle, const Float4& clearColor)
{
	auto cmd = CommandManager::GetInstance()->GetCommandList();

	auto rtvHandle = GetInstance().rtvHandleMap_.at(textureHandle);

	cmd->ClearRenderTargetView(
		FrameResourceManager::GetInstance()->GetRTVHeap()->GetCPUHandle(GetInstance().rtvHandleMap_[textureHandle]),
		&clearColor.x, 
		0, 
		nullptr
	);
}

void Cygnus::RTVManager::ClearCubeMapDepth(int32_t textureHandle)
{
	auto cmd = CommandManager::GetInstance()->GetCommandList();

	auto it = GetInstance().cubeMapDSVHandleMap_.find(textureHandle);
	if(it == GetInstance().cubeMapDSVHandleMap_.end()) return;

	cmd->ClearDepthStencilView(
		it->second,
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f, 
		0,
		0,
		nullptr
	);
}
