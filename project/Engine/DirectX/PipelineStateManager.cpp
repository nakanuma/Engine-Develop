#include "PipelineStateManager.h"

// C++
#include <format>

// Engine
#include <Logger.h>
#include <ShaderManager.h>
#include <RootSignatureManager.h>

Cygnus::PipelineStateManager* Cygnus::PipelineStateManager::GetInstance() {
	static PipelineStateManager instance;
	return &instance;
}

void Cygnus::PipelineStateManager::Initialize(
    ID3D12Device* device, 
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

    rootSignature_ = RootSignatureManager::GetInstance()->GetRootSignature(RootSignatureType::Default);

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

    // 全てのPSOを生成
	CreateAllPSOs();

    // 初期化したことをログで出力
	Cygnus::Log(std::format("PipelineStateManager initialized.\n"));
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
		Cygnus::Log(std::format("Failed to create PSO.\n"));
		return false;
		assert(0);
	}

	// PSOマップに登録
	psoMap_[name] = pso;
	return true;
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

void Cygnus::PipelineStateManager::CreateAllPSOs() {
	// 基本的なPSOを生成
	CreateBasicPSOs();
	// インスタンシング用のPSOを生成
	CreateInstancedPSOs();
	// ポストエフェクト用のPSOを生成
	CreatePostEffectPSOs();
	// 特殊な用途のPSOを生成
	CreateSpecialPSOs();
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

void Cygnus::PipelineStateManager::CreateBlendModePSOs()
{
	// PSO共通設定
	PSODescriptor baseDesc;
	baseDesc.vertexShaderName = "Object3D_VS";
	baseDesc.pixelShaderName = "Object3D_PS";
	baseDesc.rootSignature = rootSignature_;
	baseDesc.blendDesc = blendNormal_;
	baseDesc.rasterizerDesc = rasterizerDesc_;
	baseDesc.depthStencilDesc = depthStencilDesc_;
	baseDesc.inputLayout = inputLayout_;

	// 各ブレンドモード
	{
		PSODescriptor desc = baseDesc;
		desc.blendDesc = blendNone_;
		CreateAndRegisterPSO(PSOType::BlendNone, desc);
	}

	{
		PSODescriptor desc = baseDesc;
		desc.blendDesc = blendNormal_;
		CreateAndRegisterPSO(PSOType::BlendNormal, desc);
	}

	{
		PSODescriptor desc = baseDesc;
		desc.blendDesc = blendAdd_;
		CreateAndRegisterPSO(PSOType::BlendAdd, desc);
	}

	{
		PSODescriptor desc = baseDesc;
		desc.blendDesc = blendSubtract_;
		CreateAndRegisterPSO(PSOType::BlendSubtract, desc);
	}

	{
		PSODescriptor desc = baseDesc;
		desc.blendDesc = blendMultiply_;
		CreateAndRegisterPSO(PSOType::BlendMultiply, desc);
	}

	{
		PSODescriptor desc = baseDesc;
		desc.blendDesc = blendScreen_;
		CreateAndRegisterPSO(PSOType::BlendScreen, desc);
	}

	{
		PSODescriptor desc = baseDesc;
		desc.blendDesc = blendAlpha_;
		CreateAndRegisterPSO(PSOType::BlendAlpha, desc);
	}
}

void Cygnus::PipelineStateManager::CreateInstancedPSOs() {
	// PSO共通設定
	PSODescriptor baseDesc;
	baseDesc.vertexShaderName = "InstancedObject_VS";
	baseDesc.pixelShaderName = "InstancedObject_PS";
	baseDesc.rootSignature = rootSignature_;
	baseDesc.blendDesc = blendNormal_;
	baseDesc.rasterizerDesc = rasterizerDesc_;
	baseDesc.depthStencilDesc = depthStencilDesc_;
	baseDesc.inputLayout = inputLayout_;

	// 通常
	{
		PSODescriptor desc = baseDesc;
		CreateAndRegisterPSO(PSOType::InstancedObject, desc);
	}

	// 各ブレンドモード
	{
		PSODescriptor desc = baseDesc;
		desc.blendDesc = blendNone_;
		CreateAndRegisterPSO(PSOType::InstancedObjectNone, desc);
	}

	{
		PSODescriptor desc = baseDesc;
		desc.blendDesc = blendNormal_;
		CreateAndRegisterPSO(PSOType::InstancedObjectNormal, desc);
	}

	{
		PSODescriptor desc = baseDesc;
		desc.blendDesc = blendAdd_;
		CreateAndRegisterPSO(PSOType::InstancedObjectAdd, desc);
	}

	{
		PSODescriptor desc = baseDesc;
		desc.blendDesc = blendSubtract_;
		CreateAndRegisterPSO(PSOType::InstancedObjectSubtract, desc);
	}

	{
		PSODescriptor desc = baseDesc;
		desc.blendDesc = blendMultiply_;
		CreateAndRegisterPSO(PSOType::InstancedObjectMultiply, desc);
	}

	{
		PSODescriptor desc = baseDesc;
		desc.blendDesc = blendScreen_;
		CreateAndRegisterPSO(PSOType::InstancedObjectScreen, desc);
	}

	{
		PSODescriptor desc = baseDesc;
		desc.blendDesc = blendAlpha_;
		CreateAndRegisterPSO(PSOType::InstancedObjectAlpha, desc);
	}
}

void Cygnus::PipelineStateManager::CreatePostEffectPSOs() {
	// PSO共通設定
	PSODescriptor baseDesc;
	baseDesc.vertexShaderName = "Object3D_VS";	// 各PSOごとに上書き
	baseDesc.pixelShaderName = "Object3D_PS";	// 各PSOごとに上書き
	baseDesc.rootSignature = rootSignature_;
	baseDesc.blendDesc = blendNormal_;
	baseDesc.rasterizerDesc = rasterizerDesc_;
	baseDesc.depthStencilDesc = depthStencilDesc_;
	baseDesc.inputLayout = inputLayout_;

	// Grayscale
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "Grayscale_PS";
		CreateAndRegisterPSO(PSOType::Grayscale, desc);
	}

	// Vignette
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "Vignette_PS";
		CreateAndRegisterPSO(PSOType::Vignette, desc);
	}

	// DamageVignette
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "DamageVignette_PS";
		CreateAndRegisterPSO(PSOType::DamageVignette, desc);
	}

	// BoxFilter
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "BoxFilter_PS";
		CreateAndRegisterPSO(PSOType::BoxFilter, desc);
	}

	// GaussianFilter
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "GaussianFilter_PS";
		CreateAndRegisterPSO(PSOType::GaussianFilter, desc);
	}

	// GaussianHorizontal
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "GaussianHorizontal_PS";
		CreateAndRegisterPSO(PSOType::GaussianHorizontal, desc);
	}

	// GaussianVertical
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "GaussianVertical_PS";
		CreateAndRegisterPSO(PSOType::GaussianVertical, desc);
	}

	// RadialBlur
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "RadialBlur_PS";
		CreateAndRegisterPSO(PSOType::RadialBlur, desc);
	}

	// InvertColor
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "InvertColor_PS";
		CreateAndRegisterPSO(PSOType::InvertColor, desc);
	}

	// Sepia
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "Sepia_PS";
		CreateAndRegisterPSO(PSOType::Sepia, desc);
	}

	// Posterize
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "Posterize_PS";
		CreateAndRegisterPSO(PSOType::Posterize, desc);
	}

	// Emboss
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "Emboss_PS";
		CreateAndRegisterPSO(PSOType::Emboss, desc);
	}

	// Sharpen
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "Sharpen_PS";
		CreateAndRegisterPSO(PSOType::Sharpen, desc);
	}

	// ColorAberration
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "ColorAberration_PS";
		CreateAndRegisterPSO(PSOType::ColorAberration, desc);
	}

	// BarrelDistortion
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "BarrelDistortion_PS";
		CreateAndRegisterPSO(PSOType::BarrelDistortion, desc);
	}

	// WaveDistortion
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "WaveDistortion_PS";
		CreateAndRegisterPSO(PSOType::WaveDistortion, desc);
	}

	// Pixelation
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "Pixelation_PS";
		CreateAndRegisterPSO(PSOType::Pixelation, desc);
	}

	// GlitchEffect
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "GlitchEffect_PS";
		CreateAndRegisterPSO(PSOType::GlitchEffect, desc);
	}

	// BloomExtract
	{
		PSODescriptor desc = baseDesc;
		desc.pixelShaderName = "BloomExtract_PS";
		CreateAndRegisterPSO(PSOType::BloomExtract, desc);
	}

	// SobelFilter
	{
		PSODescriptor desc = baseDesc;
		desc.vertexShaderName = "SobelFilter_VS";
		desc.pixelShaderName = "SobelFilter_PS";
		CreateAndRegisterPSO(PSOType::SobelFilter, desc);
	}
}

void Cygnus::PipelineStateManager::CreateSpecialPSOs() {
	// PSO共通設定
	PSODescriptor baseDesc;
	baseDesc.vertexShaderName = "Object3D_VS"; // 各PSOごとに上書き
	baseDesc.pixelShaderName = "Object3D_PS";  // 各PSOごとに上書き
	baseDesc.rootSignature = rootSignature_;
	baseDesc.blendDesc = blendNormal_;
	baseDesc.rasterizerDesc = rasterizerDesc_;
	baseDesc.depthStencilDesc = depthStencilDesc_;
	baseDesc.inputLayout = inputLayout_;

	// Skybox
	{
		PSODescriptor desc = baseDesc;
		desc.vertexShaderName = "Skybox_VS";
		desc.pixelShaderName = "Skybox_PS";

		// 深度ステンシルに変更を加える
		desc.depthStencilDesc.DepthEnable = TRUE;
		desc.depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; // 全ピクセルがZ=1に出力されるため書き込み不要
		desc.depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL;

		CreateAndRegisterPSO(PSOType::Skybox, desc);
	}

	// Skinning
	{
		PSODescriptor desc = baseDesc;
		desc.vertexShaderName = "SkinningObject3D_VS";
		CreateAndRegisterPSO(PSOType::Skinning, desc);
	}
}

bool Cygnus::PipelineStateManager::CreateAndRegisterPSO(PSOType type, const PSODescriptor& descriptor) { 
	std::string key = PSOTypeToString(type); 
	return RegisterCustomPSO(key, descriptor);
}

std::string Cygnus::PipelineStateManager::PSOTypeToString(PSOType type) {
	switch (type) {
	// 基本
	case PSOType::Default: return "Default";

	// ブレンドモード
	case PSOType::BlendNone: return "BlendNone";
	case PSOType::BlendNormal: return "BlendNormal";
	case PSOType::BlendAdd: return "BlendAdd";
	case PSOType::BlendSubtract: return "BlendSubtract";
	case PSOType::BlendMultiply: return "BlendMultiply";
	case PSOType::BlendScreen: return "BlendScreen";
	case PSOType::BlendAlpha: return "BlendAlpha";

	// インスタンシング
	case PSOType::InstancedObject: return "InstancedObject";
	case PSOType::InstancedObjectNone: return "InstancedObjectNone";
	case PSOType::InstancedObjectNormal: return "InstancedObjectNormal";
	case PSOType::InstancedObjectAdd: return "InstancedObjectAdd";
	case PSOType::InstancedObjectSubtract: return "InstancedObjectSubtract";
	case PSOType::InstancedObjectMultiply: return "InstancedObjectMultiply";
	case PSOType::InstancedObjectScreen: return "InstancedObjectScreen";
	case PSOType::InstancedObjectAlpha: return "InstancedObjectAlpha";

	// ポストエフェクト
	case PSOType::Grayscale: return "GrayScale";
	case PSOType::Vignette:	return "Vignette";
	case PSOType::DamageVignette: return "DamageVignette";
	case PSOType::BoxFilter: return "BoxFilter";
	case PSOType::GaussianFilter: return "GaussianFilter";
	case PSOType::GaussianHorizontal: return "GaussianHorizontal";
	case PSOType::GaussianVertical: return "GaussianVertical";
	case PSOType::RadialBlur: return "RadialBlur";
	case PSOType::InvertColor: return "InvertColor";
	case PSOType::Sepia: return "Sepia";
	case PSOType::Posterize: return "Posterize";
	case PSOType::Emboss: return "Emboss";
	case PSOType::Sharpen: return "Sharpen";
	case PSOType::ColorAberration: return "ColorAberration";
	case PSOType::BarrelDistortion: return "BarrelDistortion";
	case PSOType::WaveDistortion: return "WaveDistortion";
	case PSOType::Pixelation: return "Pixelation";
	case PSOType::GlitchEffect: return "GlitchEffect";
	case PSOType::BloomExtract: return "BloomExtract";
	case PSOType::SobelFilter: return "SobelFilter";

	// 特殊用途
	case PSOType::Skybox: return "Skybox";
	case PSOType::Skinning: return "Skinning";

    default: return "Unknown";
    }
}
