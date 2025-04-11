#pragma once
#include "MyMath.h"

class Transform
{
public:
	Float3 scale;
	Float3 rotate;
	Float3 translate;

	Matrix MakeAffineMatrix();
};

