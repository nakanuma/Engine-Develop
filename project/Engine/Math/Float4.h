#pragma once

class Matrix;

struct Float4
{
	float x;
	float y;
	float z;
	float w;

	Float4 operator*(const Matrix& mat);
	Float4& operator/=(float scalar);

	static Float4 Transform(const Float4& v, const Matrix& mat);
};