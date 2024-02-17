#pragma once
#include "Color.h"
#include "Vector3.h"

struct PointLight
{
	Color mColor;
	Vector3 mPosition;
	float mIntensity;
	float mRadius;
	float mDecay;
	float mPad[2];
};
