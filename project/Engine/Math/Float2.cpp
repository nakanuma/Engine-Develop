#include "Float2.h"

Float2 Float2::operator+(const Float2& other) const { return Float2(x + other.x, y + other.y); }

Float2 Float2::operator-(const Float2& other) const { return Float2(x - other.x, y - other.y); }

Float2 Float2::operator*(float scalar) const { return Float2(x * scalar, y * scalar); }

Float2& Float2::operator+=(const Float2& other) {
	this->x += other.x;
	this->y += other.y;
	return *this;
}

float Float2::Dot(const Float2& a, const Float2& b) { return a.x * b.x + a.y * b.y; }

Float2 Float2::Lerp(const Float2& a, const Float2& b, float t) {
	return {
		a.x * (1.0f - t) + b.x * t,
		a.y * (1.0f - t) + b.y * t
	};
}