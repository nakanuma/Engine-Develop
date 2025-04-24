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
};