#include "LightManager.h"
#include "DirectXBase.h"

#include <numbers>

LightManager* LightManager::GetInstance() {
	static LightManager instance;
	return &instance;
}

void LightManager::Initialize() {
	// 平行光源のデフォルト値を書き込む
	directionalLightCB_.data_->color = kDefaultLightColor;
	directionalLightCB_.data_->direction = kDefaultDirection;
	directionalLightCB_.data_->intensity = kDefaultDirectionIntensity;

	// ポイントライトのデフォルト値を書き込む
	pointLightCB_.data_->color = kDefaultLightColor;
	pointLightCB_.data_->position = kDefaultPointPosition;
	pointLightCB_.data_->intensity = kDefaultPointIntensity;
	pointLightCB_.data_->radius = kDefaultPointRadius;
	pointLightCB_.data_->decay = kDefaultPointDecay;

	for (size_t i = 0; i < kMaxLight; i++) {
		// スポットライトのデフォルト値を書き込む
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
}

void LightManager::TransferContantBuffer() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 平行光源の定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(kRootParameterIndexDirectionalLight, directionalLightCB_.resource_->GetGPUVirtualAddress());
	// ポイントライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(kRootParameterIndexPointLight, pointLightCB_.resource_->GetGPUVirtualAddress());
	// スポットライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(kRootParameterIndexSpotLight, spotLightsCB_.resource_->GetGPUVirtualAddress());
}
