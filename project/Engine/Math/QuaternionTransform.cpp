#include "QuaternionTransform.h"

Cygnus::Matrix Cygnus::QuaternionTransform::MakeAffineMatrix() {
	Matrix result = Matrix::Identity();

	// SRTの順番で行列を生成してかける
	result *= Matrix::Scaling({scale_.x, scale_.y, scale_.z});
	result *= Matrix::QuaternionToRotation({rotate_.x, rotate_.y, rotate_.z, rotate_.w});
	result *= Matrix::Translation({translate_.x, translate_.y, translate_.z});

	return result;
}
