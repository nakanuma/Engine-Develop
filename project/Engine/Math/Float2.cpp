#include "Float2.h"

Cygnus::Float2 Cygnus::Float2::operator+(const Float2& other) const { return Float2(x + other.x, y + other.y); }

Cygnus::Float2 Cygnus::Float2::operator-(const Float2& other) const { return Float2(x - other.x, y - other.y); }

Cygnus::Float2 Cygnus::Float2::operator*(float scalar) const { return Float2(x * scalar, y * scalar); }

Cygnus::Float2& Cygnus::Float2::operator+=(const Float2& other) {
	this->x += other.x;
	this->y += other.y;
	return *this;
}

float Cygnus::Float2::Dot(Float2 a, Float2 b) { return a.x * b.x + a.y * b.y; }

Cygnus::Float2 Cygnus::Float2::Lerp(Float2 a, Float2 b, float t) {
	return {
		a.x * (1.0f - t) + b.x * t,
		a.y * (1.0f - t) + b.y * t
	};
}