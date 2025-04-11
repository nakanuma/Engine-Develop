#pragma once

struct Float2
{
	float x;
	float y;


	Float2& operator+=(const Float2& other);
};