#pragma once
#include "Float3.h"
#include "Matrix.h"
#include "Quaternion.h"

/// <summary>
/// スケール・回転（Quaternion）・平行移動をまとめた変換
/// </summary>
class QuaternionTransform {
public:
	Float3 scale;
	Quaternion rotate;
	Float3 translate;

	Matrix MakeAffineMatrix();
};
