#include "LightManager.h"
#include "DirectXBase.h"

#include <numbers>

LightManager* LightManager::GetInstance()
{
    static LightManager instance;
    return &instance;
}

void LightManager::Initialize()
{
	// 平行光源のデフォルト値を書き込む
	directionalLightCB_.data_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightCB_.data_->direction = { 0.0f, -1.0f, 0.0f };
	directionalLightCB_.data_->intensity = 1.0f;   // 平行光源有効
	/*directionalLightCB_.data_->intensity = 0.0f;*/   // 平行光源無効

	// ポイントライトのデフォルト値を書き込む
	pointLightCB_.data_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	pointLightCB_.data_->position = { 0.0f, 2.0f, 0.0f };
	/*pointLightCB_.data_->intensity = 1.0f; */   // ポイントライト有効
	pointLightCB_.data_->intensity = 0.0f;    // ポイントライト無効
	pointLightCB_.data_->radius = 5.0f;
	pointLightCB_.data_->decay = 1.0f;

	for (size_t i = 0; i < kMaxLight; i++) {
		// スポットライトのデフォルト値を書き込む
		spotLightsCB_.data_->spotLights[i].color = {1.0f, 1.0f, 1.0f, 1.0f};
		spotLightsCB_.data_->spotLights[i].position = { 0.0f, 2.0f, 0.0f }; // 初期座標
		spotLightsCB_.data_->spotLights[i].distance = 50.0f; // 大きめに設定
		spotLightsCB_.data_->spotLights[i].direction = { 0.0f, -1.0f, 0.0f }; // 真下を向ける
		spotLightsCB_.data_->spotLights[i].intensity = 4.0f;
		spotLightsCB_.data_->spotLights[i].decay = 2.0f;
		spotLightsCB_.data_->spotLights[i].cosAngle = std::cos(std::numbers::pi_v<float> / 9.0f);
		spotLightsCB_.data_->spotLights[i].cosFalloffStart = 1.26f;
		spotLightsCB_.data_->spotLights[i].isActive = false;
	}

	// 0番目のスポットライトだけ有効にする
	/*spotLightsCB_.data_->spotLights[0].isActive = true;

	spotLightsCB_.data_->spotLights[1].isActive = true;*/
}

void LightManager::TransferContantBuffer()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 平行光源の定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightCB_.resource_->GetGPUVirtualAddress());
	// ポイントライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(6, pointLightCB_.resource_->GetGPUVirtualAddress());
	// スポットライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(7, spotLightsCB_.resource_->GetGPUVirtualAddress());
}
