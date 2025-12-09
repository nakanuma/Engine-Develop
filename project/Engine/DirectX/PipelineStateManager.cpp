#include "PipelineStateManager.h"

// C++
#include <format>

// Engine
#include <Debugger/Logger.h>
#include <DirectX/ShaderManager.h>

Cygnus::PipelineStateManager* Cygnus::PipelineStateManager::GetInstance() {
	static PipelineStateManager instance;
	return &instance;
}

void Cygnus::PipelineStateManager::Initialize(
    ID3D12Device* device, 
    ID3D12RootSignature* rootSignature, 
    ID3D12RootSignature* rootSignatureParticle, 
    ID3D12RootSignature* rootSignatureInstancedObject, 
    const D3D12_INPUT_LAYOUT_DESC& inputLayout,
    const D3D12_BLEND_DESC& blendNormal, 
    const D3D12_BLEND_DESC& blendNone, 
    const D3D12_BLEND_DESC& blendAdd, 
    const D3D12_BLEND_DESC& blendSubtract, 
    const D3D12_BLEND_DESC& blendMultiply,
    const D3D12_BLEND_DESC& blendScreen, 
    const D3D12_BLEND_DESC& blendAlpha, 
    const D3D12_RASTERIZER_DESC& rasterizerDesc, 
    const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc) {
    
    device_ = device;

    rootSignature_ = rootSignature;
	rootSignatureParticle_ = rootSignatureParticle;
	rootSignatureInstancedObject_ = rootSignatureInstancedObject;

    inputLayout_ = inputLayout;
	blendNormal_ = blendNormal;
	blendNone_ = blendNone;
	blendAdd_ = blendAdd;
	blendSubtract_ = blendScreen;
	blendMultiply_ = blendMultiply;
	blendScreen_ = blendScreen;
	blendAlpha_ = blendAlpha;
	rasterizerDesc_ = rasterizerDesc;
	depthStencilDesc_ = depthStencilDesc;

    // 全ての標準PSOを生成
	CreateAllStandardPSOs();

    // 初期化したことをログで出力
	Cygnus::Log(std::format("PipelineStateManager initialized.\n"));
}

ID3D12PipelineState* Cygnus::PipelineStateManager::GetPSO(PSOType type) const { 
    // タイプを文字列キーに変換してマップを探索
    std::string key = PSOTypeToString(type); 
    auto it = psoMap_.find(key);

    // 見つからなかったらエラー
    if (it == psoMap_.end()) {
		Cygnus::Log(std::format("PSO not found.\n"));
		assert(0);
    }

    // PSOを返す
    return it->second.Get();
}

ID3D12PipelineState* Cygnus::PipelineStateManager::GetPSOByName(const std::string& name) const { 
    // 文字列キーを使用してマップを探索
    auto it = psoMap_.find(name);

    // 見つからなかったらエラー
	if (it == psoMap_.end()) {
		Cygnus::Log(std::format("PSO not found.\n"));
		assert(0);
	}

    // PSOを返す
	return it->second.Get();
}

bool Cygnus::PipelineStateManager::RegisterCustomPSO(const std::string& name, const PSODescriptor& descriptor) { 
    // 既に存在する場合はエラー
	if (psoMap_.find(name) != psoMap_.end()) {
		Cygnus::Log(std::format("PSO already exists.\n"));
		assert(0);
    }

    // 各種設定を適用してPSOを生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = descriptor.rootSignature;
	psoDesc.InputLayout = descriptor.inputLayout;

    ShaderManager* shaderManager = ShaderManager::GetInstance();
	auto vs = shaderManager->GetShader(descriptor.vertexShaderName);
	auto ps = shaderManager->GetShader(descriptor.pixelShaderName);

    // vertexShaderまたはpixelShaderが見つからなければエラー
    if (!vs || !ps) {
		Cygnus::Log(std::format("Shader not found.\n"));
		assert(0);
    }

    psoDesc.VS = {vs->GetBufferPointer(), vs->GetBufferSize()};
	psoDesc.PS = {ps->GetBufferPointer(), ps->GetBufferSize()};
	psoDesc.BlendState = descriptor.blendDesc;
	psoDesc.RasterizerState = descriptor.rasterizerDesc;
	psoDesc.DepthStencilState = descriptor.depthStencilDesc;
	psoDesc.PrimitiveTopologyType = descriptor.topologyType;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = descriptor.rtvFormat;
	psoDesc.DSVFormat = descriptor.dsvFormat;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	HRESULT result = device_->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));

	// PSO生成に失敗したらエラー
	if (FAILED(result)) {
		Cygnus::Log(std::format("Shader not found.\n"));
		return false;
		assert(0);
	}

	// PSOマップに登録
	psoMap_[name] = pso;
	return true;
}

size_t Cygnus::PipelineStateManager::GetPSOCount() const { return size_t(); }

void Cygnus::PipelineStateManager::CreateAllStandardPSOs() {
	// 基本的なPSOを生成
	CreateBasicPSOs();
	// ポストエフェクト用のPSOを生成
	CreatePostEffectPSOs();
}

void Cygnus::PipelineStateManager::CreateBasicPSOs() {
	// デフォルトPSO
	{
		PSODescriptor desc;
		desc.vertexShaderName = "Object3D_VS";
		desc.pixelShaderName = "Object3D_PS";
		desc.rootSignature = rootSignature_;
		desc.blendDesc = blendNormal_;
		desc.rasterizerDesc = rasterizerDesc_;
		desc.depthStencilDesc = depthStencilDesc_;
		desc.inputLayout = inputLayout_;

		CreateAndRegisterPSO(PSOType::Default, desc);
	}
}

void Cygnus::PipelineStateManager::CreatePostEffectPSOs() {
	
}

bool Cygnus::PipelineStateManager::CreateAndRegisterPSO(PSOType type, const PSODescriptor& descriptor) { 
	std::string key = PSOTypeToString(type); 
	return RegisterCustomPSO(key, descriptor);
}

std::string Cygnus::PipelineStateManager::PSOTypeToString(PSOType type) {
	switch (type) {
	case PSOType::Default:
		return "Default";

    default:
		return "Unknown";
    }
}
