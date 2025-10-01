#include "ShadowMapManager.h"

#include <DirectXBase.h>
#include <SRVManager.h>
#include <TextureManager.h>

using Microsoft::WRL::ComPtr;

ShadowMapManager* ShadowMapManager::GetInstance()
{
    static ShadowMapManager instance;
    return &instance;
}

int32_t ShadowMapManager::CreateShadowMap(uint32_t width, uint32_t height)
{
    DirectXBase* dxBase = DirectXBase::GetInstance();
    ID3D12Device* device = dxBase->GetDevice();

    // シャドウマップリソース作成
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
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    ComPtr<ID3D12Resource> shadowTex;
    D3D12_HEAP_PROPERTIES heapProp = {};
    heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
    device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(&shadowTex)
    );

    // DSV確保
    DescriptorHeap* dsvHeap = dxBase->GetDSVHeap();
    uint32_t dsvIndex = dsvUseIndex_++; // このクラスでインデックスを管理
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUHandle(dsvIndex);

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

    device->CreateDepthStencilView(shadowTex.Get(), &dsvDesc, dsvHandle);

    // SRV確保
    uint32_t srvIndex = TextureManager::CreateSRV(shadowTex.Get(), DXGI_FORMAT_R32_FLOAT);

    // 登録
    shadowResources_[srvIndex] = { shadowTex, dsvHandle, srvIndex, srvIndex };

    return srvIndex;
}

void ShadowMapManager::SetShadowDSV(int32_t handle)
{
    DirectXBase* dxBase = DirectXBase::GetInstance();
    ID3D12GraphicsCommandList* cmdList = dxBase->GetCommandList();

    // DSVをセット
    auto& res = shadowResources_[handle];
    cmdList->OMSetRenderTargets(0, nullptr, FALSE, &res.dsvHandle);
}

int32_t ShadowMapManager::GetShadowSRVHandle(int32_t handle)
{
    return shadowResources_[handle].srvIndex;
}

void ShadowMapManager::ClearShadowMap(int32_t handle, float clearDepth)
{
    DirectXBase* dxBase = DirectXBase::GetInstance();
    ID3D12GraphicsCommandList* cmdList = dxBase->GetCommandList();

    auto& res = shadowResources_[handle];
    cmdList->ClearDepthStencilView(
        res.dsvHandle,
        D3D12_CLEAR_FLAG_DEPTH,
        clearDepth,
        0,
        0, 
        nullptr
    );
}

void ShadowMapManager::Initialize() { 
    CreateShadowPSO(); 
    CreateShadowSkinnedPSO();
}

ID3D12Resource* ShadowMapManager::GetShadowTexture(int32_t handle) const
{
    auto it = shadowResources_.find(handle);
    if (it != shadowResources_.end()) {
        return it->second.resource.Get();
    }
    return nullptr;
}

void ShadowMapManager::TransitionShadowResource(ID3D12GraphicsCommandList* cmdList, int32_t handle, D3D12_RESOURCE_STATES newState)
{
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

void ShadowMapManager::CreateShadowPSO() {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	ID3D12Device* device = dxBase->GetDevice();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	ZeroMemory(&psoDesc, sizeof(psoDesc));
	psoDesc.pRootSignature = dxBase->GetRootSignature();

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
	for (int i = 0; i < 8; ++i)
		psoDesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
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

void ShadowMapManager::CreateShadowSkinnedPSO() {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	ID3D12Device* device = dxBase->GetDevice();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	ZeroMemory(&psoDesc, sizeof(psoDesc));
	psoDesc.pRootSignature = dxBase->GetRootSignature();

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	    {"INDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
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
	for (int i = 0; i < 8; ++i)
		psoDesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
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
