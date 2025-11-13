#include "Transform.h"

Matrix Transform::MakeAffineMatrix() {
	Matrix result = Matrix::Identity();

	// SRTの順番で行列を生成してかける
	result *= Matrix::Scaling({scale_.x, scale_.y, scale_.z});
	result *= Matrix::RotationRollPitchYaw(rotate_.z, rotate_.x, rotate_.y);
	result *= Matrix::Translation({translate_.x, translate_.y, translate_.z});

	return result;
}