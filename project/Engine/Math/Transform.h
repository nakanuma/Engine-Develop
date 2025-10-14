#pragma once
#include "MyMath.h"

/// <summary>
/// スケール・回転（オイラー角）・平行移動をまとめた変換
/// </summary>
class Transform {
public:
	Float3 scale;
	Float3 rotate;
	Float3 translate;

	Matrix MakeAffineMatrix();
};
