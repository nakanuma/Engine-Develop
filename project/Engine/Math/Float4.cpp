#include "Float4.h"

#include <Matrix.h>

Cygnus::Float4 Cygnus::Float4::operator+(const Float4& other) const { return {x + other.x, y + other.y, z + other.z, w + other.w}; }

Cygnus::Float4 Cygnus::Float4::operator-(const Float4& other) const { return {x - other.x, y - other.y, z - other.z, w - other.w}; }

Cygnus::Float4 Cygnus::Float4::operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar, w * scalar}; }

Cygnus::Float4 Cygnus::Float4::operator*(const Matrix& mat) {
	return Float4(
	    mat.r[0][0] * x + mat.r[0][1] * y + mat.r[0][2] * z + mat.r[0][3] * w, 
		mat.r[1][0] * x + mat.r[1][1] * y + mat.r[1][2] * z + mat.r[1][3] * w,
	    mat.r[2][0] * x + mat.r[2][1] * y + mat.r[2][2] * z + mat.r[2][3] * w, 
		mat.r[3][0] * x + mat.r[3][1] * y + mat.r[3][2] * z + mat.r[3][3] * w
	);
}

Cygnus::Float4& Cygnus::Float4::operator/=(float scalar) {
	x /= scalar;
	y /= scalar;
	z /= scalar;
	w /= scalar;

	return *this;
}

Cygnus::Float4 Cygnus::Float4::Transform(const Float4& v, const Matrix& m) {
	Float4 result = {};
	result.x = v.x * m.r[0][0] + v.y * m.r[1][0] + v.z * m.r[2][0] + v.w * m.r[3][0]; // X成分
	result.y = v.x * m.r[0][1] + v.y * m.r[1][1] + v.z * m.r[2][1] + v.w * m.r[3][1]; // Y成分
	result.z = v.x * m.r[0][2] + v.y * m.r[1][2] + v.z * m.r[2][2] + v.w * m.r[3][2]; // Z成分
	result.w = v.x * m.r[0][3] + v.y * m.r[1][3] + v.z * m.r[2][3] + v.w * m.r[3][3]; // W成分
	return result;
}

Cygnus::Float4 Cygnus::Float4::Lerp(const Float4& a, const Float4& b, float t) {
	return {
		a.x * (1.0f - t) + b.x * t, // X成分
		a.y * (1.0f - t) + b.y * t, // Y成分
		a.z * (1.0f - t) + b.z * t, // Z成分
		a.w * (1.0f - t) + b.w * t  // W成分
	}; 
}