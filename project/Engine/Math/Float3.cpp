#include "Float3.h"
#include "algorithm"

Float3 Float3::operator+(const Float3& other) const { 
	return {x + other.x, y + other.y, z + other.z}; }

Float3 Float3::operator-(const Float3& other) const { 
	return {x - other.x, y - other.y, z - other.z}; }

Float3 Float3::operator*(float scalar) const { 
	return {x * scalar, y * scalar, z * scalar}; }

Float3& Float3::operator+=(const Float3& other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

Float3& Float3::operator-=(const Float3& other) {
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

float Float3::Length(const Float3& v) { 
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Float3 Float3::Normalize(const Float3& a) { 
	float length = Length(a);
	return Float3(a.x / length, a.y / length, a.z / length);
}

Float3 Float3::Lerp(const Float3& a, const Float3& b, float t) { 
	return {
	    a.x * (1.0f - t) + b.x * t,
	    a.y * (1.0f - t) + b.y * t,
	    a.z * (1.0f - t) + b.z * t,
	};
}

Float3 Float3::CatmullRomInterplation(const Float3& p0, const Float3& p1, const Float3& p2, const Float3& p3, float t) { 
	const float s = 0.5f;

	float t2 = t * t;  // tの2乗
	float t3 = t2 * t; // tの3乗

	Float3 e3 = (p0 * -1.0f) + (p1 * 3.0f) - (p2 * 3.0f) + p3;
	Float3 e2 = (p0 * 2.0f) - (p1 * 5.0f) + (p2 * 4.0f) - p3;
	Float3 e1 = (p0 * -1.0f) + p2;
	Float3 e0 = (p1 * 2.0f);

	return (e3 * t3 + e2 * t2 + e1 * t + e0) * s;
}

Float3 Float3::CatmullRomPosition(const std::vector<Float3>& points, float t) { 
	assert(points.size() >= 4 && "制御点は4点以上必要です");

	// 区間数は制御点の数-1
	size_t division = points.size() - 1;
	// 1区間の長さ（全体を1.0とした割合）
	float areaWidth = 1.0f / division;

	// 区間内の始点を0.0f, 終点を1.0fとしたときの現在位置
	float t_2 = std::fmod(t, areaWidth) * division;
	// 下限（0.0f）と上限（1.0f）の範囲に収める
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	// 区間番号
	size_t index = static_cast<size_t>(t / areaWidth);
	// 区間番号が上限を超えないように収める
	index = std::min(index, division - 1);

	// 4点分のインデックス
	size_t index0 = index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;

	// 最初の区間のp0はp1を重複使用する
	if (index == 0) {
		index0 = index1;
	}

	// 最後の区間のp3はp2を重複使用する
	if (index3 >= points.size()) {
		index3 = index2;
	}

	// 4点の座標
	const Float3& p0 = points[index0];
	const Float3& p1 = points[index1];
	const Float3& p2 = points[index2];
	const Float3& p3 = points[index3];

	// 4点を指定してCatmull-Rom補間
	return CatmullRomInterplation(p0, p1, p2, p3, t_2);
}

Float3 operator*(float scalar, const Float3& vec) { 
	return {vec.x * scalar, vec.y * scalar, vec.z * scalar};
}
