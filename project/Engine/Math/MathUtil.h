#pragma once

#include <MyMath.h>
#include <Window/MyWindow.h>

class Camera;

/// <summary>
/// 便利な関数群
/// </summary>
class MathUtil {
	/// <summary>
	/// ワールド座標をスクリーン座標に変換します。
	/// </summary>
	/// <param name="worldPosition">ワールド座標</param>
	/// <returns>スクリーン座標</returns>
	static Cygnus::Float3 WorldToScreen(const Cygnus::Float3& worldPosition);
};
