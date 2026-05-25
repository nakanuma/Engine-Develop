#pragma once

#include <MyMath.h>
#include <Window/MyWindow.h>

class Camera;

namespace Cygnus {
/// <summary>
/// 便利な関数群
/// </summary>
class MathUtil {
public:
	/// <summary>
	/// ワールド座標をスクリーン座標に変換します。
	/// </summary>
	/// <param name="worldPosition">ワールド座標</param>
	/// <returns>スクリーン座標</returns>
	static Cygnus::Float3 WorldToScreen(const Cygnus::Float3& worldPosition);
};
} 
