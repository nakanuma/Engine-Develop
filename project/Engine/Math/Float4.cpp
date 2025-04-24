#include "Float4.h"

#include <Matrix.h>

Float4 Float4::operator*(const Matrix& mat) { 
	return Float4(
		mat.r[0][0] * x + mat.r[0][1] * y + mat.r[0][2] * z + mat.r[0][3] * w,
		mat.r[1][0] * x + mat.r[1][1] * y + mat.r[1][2] * z + mat.r[1][3] * w,
		mat.r[2][0] * x + mat.r[2][1] * y + mat.r[2][2] * z + mat.r[2][3] * w,
		mat.r[3][0] * x + mat.r[3][1] * y + mat.r[3][2] * z + mat.r[3][3] * w
	);
}

Float4& Float4::operator/=(float scalar) {
	x /= scalar;
	y /= scalar;
	z /= scalar;
	w /= scalar;

	return *this;
}
