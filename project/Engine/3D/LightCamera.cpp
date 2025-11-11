#include "LightCamera.h"

#include <algorithm>

LightCamera* LightCamera::GetInstance() {
	static LightCamera instance;
	return &instance;
}

void LightCamera::SetDirectionalLight(const Float3& dir) {
	lightDir_ = dir;
	lightDir_ = Float3::Normalize(lightDir_);
}

void LightCamera::UpdateViewProjection(const BoundingBox& sceneBB) {
	// 平行光源位置はシーン中心からライト方向に適度に離す
	Float3 center = sceneBB.Center();
	Float3 pos = center - lightDir_ * 50.0f; // 影の範囲による

	Float3 forward = Float3::Normalize(lightDir_ * -1.0f); // ライトの届く方向を視線方向に

	Float3 worldUp = {0, 1, 0};
	Float3 right = Float3::Cross(worldUp, lightDir_);
	if (Float3::LengthSq(right) < 1e-6f) {
		// LightDirとworldUpがほぼ平行ならX軸をUpにする
		worldUp = {1, 0, 0};
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
	float nearZ = max(0.1f, minV.z);
	float farZ = maxV.z + 250.0f; // Zレンジが極端に狭くならないよう、マージンを足す

	// 直交射影行列
	proj_ = Matrix::OrthographicOffCenterLH(minV.x, maxV.x, minV.y, maxV.y, nearZ, farZ);

	// 合成行列
	viewProj_ = view_ * proj_;
}

void LightCamera::TransferConstantBuffer() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	cb_.data_->lightViewProj = viewProj_;
	// ルートシグネチャに定数バッファをバインド
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(13, cb_.resource_->GetGPUVirtualAddress());
}