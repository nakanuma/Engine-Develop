#include "LightManager.h"
#include "DirectXBase.h"

#include <numbers>

LightManager* LightManager::GetInstance() {
	static LightManager instance;
	return &instance;
}

void LightManager::Initialize() {
	// 平行光源の初期化
	directionalLightCB_.data_->color = kDefaultLightColor;
	directionalLightCB_.data_->direction = kDefaultDirection;
	directionalLightCB_.data_->intensity = kDefaultDirectionIntensity;

	// ポイントライトの初期化
	pointLightCB_.data_->color = kDefaultLightColor;
	pointLightCB_.data_->position = kDefaultPointPosition;
	pointLightCB_.data_->intensity = kDefaultPointIntensity;
	pointLightCB_.data_->radius = kDefaultPointRadius;
	pointLightCB_.data_->decay = kDefaultPointDecay;

	// スポットライトの初期化
	for (size_t i = 0; i < kMaxLight; i++) {
		spotLightsCB_.data_->spotLights[i].color = kDefaultLightColor;
		spotLightsCB_.data_->spotLights[i].position = kDefaultSpotPosition;
		spotLightsCB_.data_->spotLights[i].distance = kDefaultSpotDistance;
		spotLightsCB_.data_->spotLights[i].direction = kDefaultSpotDirection;
		spotLightsCB_.data_->spotLights[i].intensity = kDefaultSpotIntensity;
		spotLightsCB_.data_->spotLights[i].decay = kDefaultSpotDecay;
		spotLightsCB_.data_->spotLights[i].cosAngle = kDefaultSpotCosAngle;
		spotLightsCB_.data_->spotLights[i].cosFalloffStart = kDefaultSpotCosFalloffStart;
		spotLightsCB_.data_->spotLights[i].isActive = false;
	}

	// エミッシブライトの初期化
	for(size_t i = 0; i < kMaxEmissiveLight; i++){
		emissiveLightsCB_.data_->emissiveLights[i].color = kDefaultEmissiveColor;
		emissiveLightsCB_.data_->emissiveLights[i].position = kDefaultEmissivePosition;
		emissiveLightsCB_.data_->emissiveLights[i].intensity = kDefaultEmissiveIntensity;
		emissiveLightsCB_.data_->emissiveLights[i].radius = kDefaultEmissiveRadius;
		emissiveLightsCB_.data_->emissiveLights[i].decay = kDefaultEmissiveDecay;
		emissiveLightsCB_.data_->emissiveLights[i].isActive = false;
	}
	emissiveLightsCB_.data_->numActiveLights = 0;
	// カウンターを初期化
	currentEmissiveLightCount_ = 0;
}

void LightManager::TransferContantBuffer() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 平行光源の定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(kRootParameterIndexDirectionalLight, directionalLightCB_.resource_->GetGPUVirtualAddress());
	// ポイントライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(kRootParameterIndexPointLight, pointLightCB_.resource_->GetGPUVirtualAddress());
	// スポットライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(kRootParameterIndexSpotLight, spotLightsCB_.resource_->GetGPUVirtualAddress());
	// エミッシブライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(kRootParameterIndexEmissiveLight, emissiveLightsCB_.resource_->GetGPUVirtualAddress());
}

void LightManager::RegisterEmissiveLight(const Float3& position, const Float3& color, float intensity, float radius, float decay)
{
	// 最大数を超えていたら登録しない
	if(currentEmissiveLightCount_ >= kMaxEmissiveLight){
		return;
	}

	// エミッシブライトを登録
	EmissiveLight& light = emissiveLightsCB_.data_->emissiveLights[currentEmissiveLightCount_];
	light.color = Float4{color.x, color.y, color.z, 1.0f};
	light.position = position;
	light.intensity = intensity;
	light.radius = radius;
	light.decay = decay;
	light.isActive = true;

	currentEmissiveLightCount_++;
	emissiveLightsCB_.data_->numActiveLights = currentEmissiveLightCount_;
}

void LightManager::ClearEmissiveLights()
{
	// 毎フレーム開始時に呼び出してリセット
	for(int32_t i = 0; i < currentEmissiveLightCount_; i++){
		emissiveLightsCB_.data_->emissiveLights[i].isActive = 0;
	}
	currentEmissiveLightCount_ = 0;
	emissiveLightsCB_.data_->numActiveLights = 0;
}
