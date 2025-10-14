#pragma once
#include "Float2.h"
#include "Float3.h"
#include "Float4.h"
#include "Quaternion.h"

/// <summary>
/// 4x4行列
/// </summary>
class Matrix {
public:
	float r[4][4];

	///
	/// Constructors
	///

	// 単位行列で初期化
	Matrix();

	// floatを16個で初期化
	Matrix(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33);

	///
	/// Operators
	///

	Matrix operator-() const;

	Matrix operator+(const Matrix& m) const;
	Matrix operator-(const Matrix& m) const;
	Matrix operator*(const Matrix& m) const;

	Matrix& operator+=(const Matrix& m);
	Matrix& operator-=(const Matrix& m);
	Matrix& operator*=(const Matrix& m);

	Float4 operator*(const Float4& vec);

	///
	/// Functions
	///

	static Matrix Identity();

	static Matrix Inverse(Matrix m);

	static Matrix Transpose(const Matrix& m);

	static Matrix PerspectiveFovLH(float fov, float aspectRatio, float nearZ, float farZ);

	static Matrix Orthographic(float width, float height, float nearClip, float farClip);

	static Matrix OrthographicOffCenterLH(float left, float right, float bottom, float top, float nearZ, float farZ);

	static Matrix LookAtLH(const Float3& eye, const Float3& target, const Float3& up);

	static Matrix Scaling(Float3 scale);

	static Matrix Translation(Float3 translation);

	static Matrix RotationX(float rad);
	static Matrix RotationY(float rad);
	static Matrix RotationZ(float rad);

	static Matrix Rotation(const Float3& eulerAngles);

	static Matrix Pitch(float rad);
	static Matrix Yaw(float rad);
	static Matrix Roll(float rad);

	static Matrix LookRotation(const Float3& forward, const Float3& up);

	static Matrix RotationRollPitchYaw(float roll, float pitch, float yaw);

	static Matrix QuaternionToRotation(Quaternion q);

	static Matrix MakeAffine(const Float3& scale, const Quaternion& rotate, const Float3 translate);
};
