#include "MathUtil.h"

#include <Camera.h>

Cygnus::Float3 Cygnus::MathUtil::WorldToScreen(const Cygnus::Float3& worldPosition) { 
	// ビュー射影行列を取得
	Cygnus::Matrix worldViewProjMatrix = Cygnus::Camera::GetCurrent()->GetViewProjectionMatrix();
	// ワールド->クリップへの座標変換
	Cygnus::Float3 screenPosition = Cygnus::Float3::Transform(worldPosition, worldViewProjMatrix);
	// NDC->スクリーンへの座標変換
	screenPosition.x = (screenPosition.x + 1.0f) * 0.5f * Cygnus::Window::GetWidth();
	screenPosition.y = (1.0f - screenPosition.y) * 0.5f * Cygnus::Window::GetHeight();

	return screenPosition;
}
