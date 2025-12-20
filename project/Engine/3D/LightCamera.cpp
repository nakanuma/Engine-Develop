#include "LightCamera.h"

// C++
#include <algorithm>

// Engine
#include <CommandManager.h>

Cygnus::LightCamera* Cygnus::LightCamera::GetInstance() {
	static Cygnus::LightCamera instance;
	return &instance;
}

void Cygnus::LightCamera::SetDirectionalLight(const Float3& dir) {
	lightDir_ = dir;
	lightDir_ = Float3::Normalize(lightDir_);
}

void Cygnus::LightCamera::UpdateViewProjection(const BoundingBox& sceneBB) {
	// 平行光源位置はシーン中心からライト方向に適度に離す
	Float3 center = sceneBB.Center();
	Float3 pos = center - lightDir_ * kLightPosDistance;

	Float3 forward = Float3::Normalize(lightDir_ * -1.0f); // ライトの届く方向を視線方向に

	Float3 worldUp = { 0.0f, 1.0f, 0.0f };
	Float3 right = Float3::Cross(worldUp, lightDir_);
	if (Float3::LengthSq(right) < kThreshold) {
		// LightDirとworldUpがほぼ平行ならX軸をUpにする
		worldUp = { 1.0f, 0.0f, 0.0f };
		right = Float3::Cross(worldUp, lightDir_);
	}
	Float3 up = Float3::Cross(lightDir_, worldUp);

	// ビュー行列
	view_ = Matrix::LookAtLH(pos, center, up);

	// シーンBBをライト空間に変換
	Float3 minV(FLT_MAX, FLT_MAX, FLT_MAX);
	Float3 maxV(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	Float3 corners[8];
	sceneBB.GetCorners(corners);

	for (int i = 0; i < 8; i++) {
		Float3 v = Float3::Transform(corners[i], view_);
		minV = Float3::Min(minV, v);
		maxV = Float3::Max(maxV, v);
	}

	// near/far はライト空間Zに合わせる
	float nearZ = max(kNearZMin, minV.z);
	float farZ = maxV.z + kFarZMargin; // Zレンジが極端に狭くならないよう、マージンを足す

	// 直交射影行列
	proj_ = Matrix::OrthographicOffCenterLH(minV.x, maxV.x, minV.y, maxV.y, nearZ, farZ);

	// 合成行列
	viewProj_ = view_ * proj_;
}

void Cygnus::LightCamera::TransferConstantBuffer() {
	auto cmd = CommandManager::GetInstance()->GetCommandList();

	cb_.data_->lightViewProj = viewProj_;
	// ルートシグネチャに定数バッファをバインド
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexCBV, cb_.resource_->GetGPUVirtualAddress());
}