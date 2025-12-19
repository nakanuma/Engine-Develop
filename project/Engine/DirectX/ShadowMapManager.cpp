#include "ShadowMapManager.h"

// Engine
#include <DirectXBase.h>
#include <LightCamera.h>
#include <SRVManager.h>
#include <TextureManager.h>
#include <PipelineStateManager.h>
#include <RootSignatureManager.h>
#include <FrameResourceManager.h>

using Microsoft::WRL::ComPtr;

Cygnus::ShadowMapManager* Cygnus::ShadowMapManager::GetInstance() {
	static ShadowMapManager instance;
	return &instance;
}

void Cygnus::ShadowMapManager::Initialize() {
	// 通常オブジェクト用PSO生成
	CreateShadowPSO();
	// スキニング用PSO生成
	CreateShadowSkinnedPSO();
}

int32_t Cygnus::ShadowMapManager::CreateShadowMap(uint32_t width, uint32_t height) {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	ID3D12Device* device = dxBase->GetDevice();

	// シャドウマップ用テクスチャ作成
	D3D12_RESOURCE_DESC texDesc{};
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS; // 深度+SRV用
	texDesc.SampleDesc.Count = 1;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = kDefaultClearDepth;
	clearValue.DepthStencil.Stencil = 0;

	ComPtr<ID3D12Resource> shadowTex;
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&shadowTex));

	// DSVを作成
	DescriptorHeap* dsvHeap = FrameResourceManager::GetInstance()->GetDSVHeap();
	uint32_t dsvIndex = dsvUseIndex_++; // このクラスでインデックスを管理
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUHandle(dsvIndex);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	device->CreateDepthStencilView(shadowTex.Get(), &dsvDesc, dsvHandle);

	// SRVを作成
	uint32_t srvIndex = TextureManager::CreateSRV(shadowTex.Get(), DXGI_FORMAT_R32_FLOAT);

	// シャドウリソースを管理リストに登録
	shadowResources_[srvIndex] = {shadowTex, dsvHandle, srvIndex, srvIndex};

	return srvIndex;
}

void Cygnus::ShadowMapManager::BeginShadowPass(uint32_t shadowMapHandle) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// シャドウマップ用PSOをセット
	dxBase->GetCommandList()->SetPipelineState(ShadowMapManager::GetInstance()->GetShadowPSO());

	// Viewport / Scissor の設定
	D3D12_VIEWPORT vp{};
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<float>(Window::GetWidth());
	vp.Height = static_cast<float>(Window::GetHeight());
	vp.MinDepth = kMinDepth;
	vp.MaxDepth = kMaxDepth;
	dxBase->GetCommandList()->RSSetViewports(1, &vp);

	D3D12_RECT sc{};
	sc.left = 0;
	sc.top = 0;
	sc.right = static_cast<LONG>(Window::GetWidth());
	sc.bottom = static_cast<LONG>(Window::GetHeight());
	dxBase->GetCommandList()->RSSetScissorRects(1, &sc);

	// シャドウマップ書き込み前に書き込み状態に遷移
	TransitionShadowResource(dxBase->GetCommandList(), shadowMapHandle, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	// シャドウマップDSVをセット
	SetShadowDSV(shadowMapHandle);

	// シャドウマップをクリア
	ClearShadowMap(shadowMapHandle);
}

void Cygnus::ShadowMapManager::EndShadowPass(uint32_t shadowMapHandle) {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	FrameResourceManager* frameResource = FrameResourceManager::GetInstance();

	// 描画後、SRVとして使えるように遷移
	ShadowMapManager::GetInstance()->TransitionShadowResource(dxBase->GetCommandList(), shadowMapHandle, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	// ShadowMapをバインド
	TextureManager::SetDescriptorTable(kRootParameterIndexShadowMap, dxBase->GetCommandList(), shadowMapHandle);
	// LightCameraの定数バッファを送信（PixelShader内で使用）
	LightCamera::GetInstance()->TransferConstantBuffer();

	// バックバッファ用PSOに切り替え
	dxBase->GetCommandList()->SetPipelineState(PipelineStateManager::GetInstance()->GetPSO(PSOType::Default));
	// バックバッファDSVに切り替え
	UINT backBufferIndex = dxBase->GetSwapChain()->GetCurrentBackBufferIndex();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = frameResource->GetRTVHandle(backBufferIndex);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = frameResource->GetDSVHeap()->GetCPUHandle(0);
	dxBase->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

int32_t Cygnus::ShadowMapManager::GetShadowSRVHandle(int32_t handle) { return shadowResources_[handle].srvIndex; }

ID3D12Resource* Cygnus::ShadowMapManager::GetShadowTexture(int32_t handle) const {
	auto it = shadowResources_.find(handle);
	if (it != shadowResources_.end()) {
		return it->second.resource.Get();
	}
	return nullptr;
}

void Cygnus::ShadowMapManager::SetShadowDSV(int32_t handle) {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	ID3D12GraphicsCommandList* cmdList = dxBase->GetCommandList();

	// DSVをセット
	auto& res = shadowResources_[handle];
	cmdList->OMSetRenderTargets(0, nullptr, FALSE, &res.dsvHandle);
}

void Cygnus::ShadowMapManager::ClearShadowMap(int32_t handle, float clearDepth) {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	ID3D12GraphicsCommandList* cmdList = dxBase->GetCommandList();

	auto& res = shadowResources_[handle];
	cmdList->ClearDepthStencilView(res.dsvHandle, D3D12_CLEAR_FLAG_DEPTH, clearDepth, 0, 0, nullptr);
}

void Cygnus::ShadowMapManager::TransitionShadowResource(ID3D12GraphicsCommandList* cmdList, int32_t handle, D3D12_RESOURCE_STATES newState) {
	auto& shadow = shadowResources_.at(handle);
	if (shadow.currentState != newState) {
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = shadow.resource.Get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = shadow.currentState;
		barrier.Transition.StateAfter = newState;

		cmdList->ResourceBarrier(1, &barrier);

		// 状態を更新
		shadow.currentState = newState;
	}
}

void Cygnus::ShadowMapManager::CreateShadowPSO() {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	ID3D12Device* device = dxBase->GetDevice();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	ZeroMemory(&psoDesc, sizeof(psoDesc));
	psoDesc.pRootSignature = RootSignatureManager::GetInstance()->GetRootSignature(RootSignatureType::Default);

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
    };
	psoDesc.InputLayout = {inputLayout, _countof(inputLayout)};
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.DepthStencilState.DepthEnable = TRUE;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	psoDesc.DepthStencilState.StencilEnable = FALSE;

	psoDesc.NumRenderTargets = 0; // カラーターゲットなし
	for (int i = 0; i < kMaxRenderTargets; ++i) psoDesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	auto vs = ShaderManager::GetInstance()->GetShader("ShadowMap_VS");
	psoDesc.VS = {vs->GetBufferPointer(), vs->GetBufferSize()};
	psoDesc.PS = {nullptr, 0};

	psoDesc.SampleMask = UINT_MAX;
	psoDesc.SampleDesc.Count = 1;

	HRESULT hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&shadowPSO_));
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create shadow map PSO");
	}
}

void Cygnus::ShadowMapManager::CreateShadowSkinnedPSO() {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	ID3D12Device* device = dxBase->GetDevice();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	ZeroMemory(&psoDesc, sizeof(psoDesc));
	psoDesc.pRootSignature = RootSignatureManager::GetInstance()->GetRootSignature(RootSignatureType::Default);

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	    {"WEIGHT",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	    {"INDEX",    0, DXGI_FORMAT_R32G32B32A32_SINT,  1, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
    };

	psoDesc.InputLayout = {inputLayout, _countof(inputLayout)};
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.DepthStencilState.DepthEnable = TRUE;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	psoDesc.DepthStencilState.StencilEnable = FALSE;

	psoDesc.NumRenderTargets = 0; // カラーターゲットなし
	for (int i = 0; i < kMaxRenderTargets; ++i) psoDesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	auto vs = ShaderManager::GetInstance()->GetShader("ShadowMapSkinned_VS");
	psoDesc.VS = {vs->GetBufferPointer(), vs->GetBufferSize()};
	psoDesc.PS = {nullptr, 0};

	psoDesc.SampleMask = UINT_MAX;
	psoDesc.SampleDesc.Count = 1;

	HRESULT hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&shadowSkinnedPSO_));
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create shadow map PSO");
	}
}
