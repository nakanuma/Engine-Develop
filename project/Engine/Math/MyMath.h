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

static constexpr double PI = 3.14159265359;		/* double型の円周率 */
static constexpr float PIf = 3.14159265359f;	/* float型の円周率 */

/// <summary>
/// 度をラジアンに変換します。
/// </summary>
/// <param name="degree">変換する角度（度）</param>
/// <returns>変換後の角度（ラジアン）</returns>
static float DegToRad(float degree) { return degree * (PIf / 180.0f); }
