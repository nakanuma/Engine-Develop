#include "Float2.h"

Float2 Float2::operator+(const Float2& other) const 
{ 
	return Float2(x + other.x, y + other.y); 
}

Float2 Float2::operator-(const Float2& other) const 
{ 
	return Float2(x - other.x, y - other.y); 
}

Float2& Float2::operator+=(const Float2& other) 
{
	this->x += other.x;
	this->y += other.y;
	return *this;
}

float Float2::Dot(const Float2& a, const Float2& b)
{
	return a.x * b.x + a.y * b.y;
}