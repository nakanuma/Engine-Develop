#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include "Float2.h"
#include "Float3.h"
#include "Float4.h"
#include "Matrix.h"
#include "Matrix3x3.h"
#include "Quaternion.h"
#include "QuaternionTransform.h"
#include "Transform.h"

#include <Camera.h>

namespace Cygnus {
static constexpr double PI = 3.14159265359;		/* double型の円周率 */
static constexpr float PIf = 3.14159265359f;	/* float型の円周率 */

/// <summary>
/// 度をラジアンに変換します。
/// </summary>
/// <param name="degree">変換する角度（度）</param>
/// <returns>変換後の角度（ラジアン）</returns>
static float DegToRad(float degree) { return degree * (PIf / 180.0f); }

/// <summary>
/// ワールド座標をスクリーン座標に変換します。
/// </summary>
/// <param name="worldPosition">ワールド座標</param>
/// <returns>スクリーン座標</returns>
static Cygnus::Float3 WorldToScreen(const Cygnus::Float3& worldPosition) {
	// ビュー射影行列を取得
	Cygnus::Matrix worldViewProjMatrix = Cygnus::Camera::GetCurrent()->GetViewProjectionMatrix();
	// ワールド->クリップへの座標変換
	Cygnus::Float3 screenPosition = Cygnus::Float3::Transform(worldPosition, worldViewProjMatrix);
	// NDC->スクリーンへの座標変換
	screenPosition.x = (screenPosition.x + 1.0f) * 0.5f * Cygnus::Window::GetWidth();
	screenPosition.y = (1.0f - screenPosition.y) * 0.5f * Cygnus::Window::GetHeight();

	return screenPosition;
}
}
