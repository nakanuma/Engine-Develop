#include "RootSignatureManager.h"

// C++
#include <format>
#include <cassert>

// Engine
#include <Logger.h>
#include "CommandManager.h"

Cygnus::RootSignatureManager* Cygnus::RootSignatureManager::GetInstance() {
	static RootSignatureManager instance;
	return &instance;
}

void Cygnus::RootSignatureManager::Initialize(ID3D12Device* device)
{
	device_ = device;

	// 全てのRootSignatureを生成
	CreateAllRootSignatures();

	// 初期化したことをログで出力
	Cygnus::Log(std::format("RootSignatureManager initialized.\n"));
}

bool Cygnus::RootSignatureManager::RegisterCustomRootSignature(const std::string& name, const RootSignatureDescriptor& descriptor)
{
	// 既に存在する場合はエラー
	if (rootSignatureMap_.find(name) != rootSignatureMap_.end()) {
		Log(std::format("Rootsignature already exists.\n"));
		assert(0);
	}

	// D3D12_ROOT_SIGNATURE_DESCを構築
	D3D12_ROOT_SIGNATURE_DESC d3d12Desc = BuildD3D12Desc(descriptor);

	// シリアライズ
	signatureBlob_ = nullptr;
	errorBlob_ = nullptr;

	HRESULT result = D3D12SerializeRootSignature(
		&d3d12Desc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob_,
		&errorBlob_
	);

	// 失敗したらエラー
	if (FAILED(result)) {
		Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(0);
	}

	// RootSignature生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	result = device_->CreateRootSignature(
		0,
		signatureBlob_->GetBufferPointer(),
		signatureBlob_->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)
	);

	// 失敗したらエラー
	if(FAILED(result)){
		Log(std::format("Failed to create RootSignature\n"));
		assert(0);
	}

	// RootSignatureマップに登録
	rootSignatureMap_[name] = rootSignature;
	return true;
}

ID3D12RootSignature* Cygnus::RootSignatureManager::GetRootSignature(RootSignatureType type) const
{
	std::string key = RootSignatureTypeToString(type);
	auto it = rootSignatureMap_.find(key);

	// 見つからなかったらエラー
	if(it == rootSignatureMap_.end()){
		Log(std::format("Rootsignature not found.\n"));
		assert(0);
	}

	// RootSignatureを返す
	return it->second.Get();
}

ID3D12RootSignature* Cygnus::RootSignatureManager::GetRootSignatureByName(const std::string& name) const
{
	auto it = rootSignatureMap_.find(name);

	// 見つからなかったらエラー
	if(it == rootSignatureMap_.end()){
		Log(std::format("Rootsignature not found.\n"));
		assert(0);
	}

	// RootSignatureを返す
	return it->second.Get();
}

void Cygnus::RootSignatureManager::CreateAllRootSignatures()
{
	// デフォルトのRootSignatureを生成
	CreateDefaultRootSignature();
}

void Cygnus::RootSignatureManager::CreateDefaultRootSignature()
{
	RootSignatureDescriptor desc;

	// DescriptorRange作成
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges(kDefaultDescriptorRangeCount);

	// t0 : 通常テクスチャ
	descriptorRanges[0].BaseShaderRegister = 0;
	descriptorRanges[0].NumDescriptors = 1;
	descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// t1 : structuredBuffer
	descriptorRanges[1].BaseShaderRegister = 1;
	descriptorRanges[1].NumDescriptors = 1;
	descriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// TextureCube用に独立したDescriptorRange作成
	D3D12_DESCRIPTOR_RANGE cubeMapRange{};
	cubeMapRange.BaseShaderRegister = 2;
	cubeMapRange.NumDescriptors = 1;
	cubeMapRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	cubeMapRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ShadowMap用に独立したDescriptorRange作成
	D3D12_DESCRIPTOR_RANGE shadowMapRange{};
	shadowMapRange.BaseShaderRegister = 3;
	shadowMapRange.NumDescriptors = 1;
	shadowMapRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	shadowMapRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// RootParameter作成
	desc.rootParameters.resize(kDefaultRootParameterCount);

	// [0] Material（CBV）
	desc.rootParameters[kRootParameterIndexMaterial].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.rootParameters[kRootParameterIndexMaterial].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	desc.rootParameters[kRootParameterIndexMaterial].Descriptor.ShaderRegister = kMaterialCBVRegister;

	// [1] TransformationMatrix（CBV）
	desc.rootParameters[kRootParameterIndexTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.rootParameters[kRootParameterIndexTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	desc.rootParameters[kRootParameterIndexTransform].Descriptor.ShaderRegister = kTransformCBVRegister;

	// [2] DescriptorTable
	desc.rootParameters[kRootParameterIndexDescriptorTable].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	desc.rootParameters[kRootParameterIndexDescriptorTable].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	desc.rootParameters[kRootParameterIndexDescriptorTable].DescriptorTable.pDescriptorRanges = descriptorRanges.data();
	desc.rootParameters[kRootParameterIndexDescriptorTable].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(descriptorRanges.size());
	
	// [3] DirecitonalLight（CBV）
	desc.rootParameters[kRootParameterIndexDirectionalLight].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
	desc.rootParameters[kRootParameterIndexDirectionalLight].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	desc.rootParameters[kRootParameterIndexDirectionalLight].Descriptor.ShaderRegister = kDirectionalLightCBVRegister;

	// [4] Camera（CBV）
	desc.rootParameters[kRootParameterIndexCamera].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.rootParameters[kRootParameterIndexCamera].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	desc.rootParameters[kRootParameterIndexCamera].Descriptor.ShaderRegister = kCameraCBVRegister;
	
	// [5] StructuredBuffer
	desc.rootParameters[kRootParameterIndexStrcturedBuffer].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	desc.rootParameters[kRootParameterIndexStrcturedBuffer].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	desc.rootParameters[kRootParameterIndexStrcturedBuffer].DescriptorTable.pDescriptorRanges = &descriptorRanges[0];
	desc.rootParameters[kRootParameterIndexStrcturedBuffer].DescriptorTable.NumDescriptorRanges = 1;
	
	// [6] PointLight（CBV）
	desc.rootParameters[kRootParameterIndexPointLight].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.rootParameters[kRootParameterIndexPointLight].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	desc.rootParameters[kRootParameterIndexPointLight].Descriptor.ShaderRegister = kPointLightCBVRegister;
	
	// [7] SpotLight（CBV）
	desc.rootParameters[kRootParameterIndexSpotLight].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.rootParameters[kRootParameterIndexSpotLight].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	desc.rootParameters[kRootParameterIndexSpotLight].Descriptor.ShaderRegister = kSpotLightCBVRegister;
	
	// [8] CubeMap
	desc.rootParameters[kRootParameterIndexCubeMap].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	desc.rootParameters[kRootParameterIndexCubeMap].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	desc.rootParameters[kRootParameterIndexCubeMap].DescriptorTable.pDescriptorRanges = &cubeMapRange;
	desc.rootParameters[kRootParameterIndexCubeMap].DescriptorTable.NumDescriptorRanges = 1;
	
	// [9] WaveDistoration（CBV）
	desc.rootParameters[kRootParameterIndexWaveDistortion].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.rootParameters[kRootParameterIndexWaveDistortion].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	desc.rootParameters[kRootParameterIndexWaveDistortion].Descriptor.ShaderRegister = kWaveDistortionCBVRegister;
	
	// [10] GlitchEffect（CBV）
	desc.rootParameters[kRootParameterIndexGlitchEffect].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.rootParameters[kRootParameterIndexGlitchEffect].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	desc.rootParameters[kRootParameterIndexGlitchEffect].Descriptor.ShaderRegister = kWaveGlitchEffectCBVRegister;
	
	// [11] LightCameraObject（CBV）
	desc.rootParameters[kRootParameterIndexLightCamera].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.rootParameters[kRootParameterIndexLightCamera].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	desc.rootParameters[kRootParameterIndexLightCamera].Descriptor.ShaderRegister = 1;
	
	// [12] ShadowMap
	desc.rootParameters[kRootParameterIndexShadowMap].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	desc.rootParameters[kRootParameterIndexShadowMap].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	desc.rootParameters[kRootParameterIndexShadowMap].DescriptorTable.pDescriptorRanges = &shadowMapRange;
	desc.rootParameters[kRootParameterIndexShadowMap].DescriptorTable.NumDescriptorRanges = 1;
	
	// [13] LightViewProj（CBV）
	desc.rootParameters[kRootParameterIndexLightViewProj].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.rootParameters[kRootParameterIndexLightViewProj].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	desc.rootParameters[kRootParameterIndexLightViewProj].Descriptor.ShaderRegister = kLightViewProjCBVRegister;
	
	// [14] EmissiveLight（CBV）
	desc.rootParameters[kRootParameterIndexEmissiveLight].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.rootParameters[kRootParameterIndexEmissiveLight].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	desc.rootParameters[kRootParameterIndexEmissiveLight].Descriptor.ShaderRegister = kEmissiveLightCBVRegister;
	
	// [15] AreaLight（CBV）
	desc.rootParameters[kRootParameterIndexAreaLight].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.rootParameters[kRootParameterIndexAreaLight].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	desc.rootParameters[kRootParameterIndexAreaLight].Descriptor.ShaderRegister = kAreaLightCBVRegister;
	
	// [16] DamageVignette（CBV）
	desc.rootParameters[kRootParameterIndexDamageVignette].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.rootParameters[kRootParameterIndexDamageVignette].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	desc.rootParameters[kRootParameterIndexDamageVignette].Descriptor.ShaderRegister = kDamageVignetteCBVRegister;

	// StaticSampler作成
	desc.staticSamplers.resize(kDefaultStaticSamplerCount);

	// [0] 通常テクスチャ用サンプラー
	desc.staticSamplers[kNormalSamplerRegister].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
	desc.staticSamplers[kNormalSamplerRegister].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	desc.staticSamplers[kNormalSamplerRegister].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	desc.staticSamplers[kNormalSamplerRegister].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	desc.staticSamplers[kNormalSamplerRegister].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	desc.staticSamplers[kNormalSamplerRegister].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	desc.staticSamplers[kNormalSamplerRegister].ShaderRegister = kNormalSamplerRegister;
	desc.staticSamplers[kNormalSamplerRegister].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	
	// [1] シャドウマップ用比較サンプラー
	desc.staticSamplers[kShadowSamplerRegister].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // 比較サンプラー
	desc.staticSamplers[kShadowSamplerRegister].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER; // 外側は白
	desc.staticSamplers[kShadowSamplerRegister].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	desc.staticSamplers[kShadowSamplerRegister].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	desc.staticSamplers[kShadowSamplerRegister].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // 典型的シャドウ判定
	desc.staticSamplers[kShadowSamplerRegister].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	desc.staticSamplers[kShadowSamplerRegister].ShaderRegister = kShadowSamplerRegister;
	desc.staticSamplers[kShadowSamplerRegister].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	// Flags設定
	desc.flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// 登録
	CreateAndRegisterRootSignature(RootSignatureType::Default, desc);
}

bool Cygnus::RootSignatureManager::CreateAndRegisterRootSignature(RootSignatureType type, const RootSignatureDescriptor& descriptor)
{
	std::string key = RootSignatureTypeToString(type);
	return RegisterCustomRootSignature(key, descriptor);
}

std::string Cygnus::RootSignatureManager::RootSignatureTypeToString(RootSignatureType type)
{
	switch(type){
	// 基本
	case RootSignatureType::Default: return "Default";

	default: return "Unknown";
	}
}

D3D12_ROOT_SIGNATURE_DESC Cygnus::RootSignatureManager::BuildD3D12Desc(const RootSignatureDescriptor& descriptor)
{
	D3D12_ROOT_SIGNATURE_DESC desc{};
	desc.Flags = descriptor.flags;
	desc.pParameters = descriptor.rootParameters.data();
	desc.NumParameters = static_cast<UINT>(descriptor.rootParameters.size());
	desc.pStaticSamplers = descriptor.staticSamplers.data();
	desc.NumStaticSamplers = static_cast<UINT>(descriptor.staticSamplers.size());

	return desc;
}
