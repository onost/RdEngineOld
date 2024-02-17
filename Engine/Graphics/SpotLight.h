#pragma once
#include "Color.h"
#include "Vector3.h"

struct SpotLight
{
	Color mColor;
	Vector3 mDirection;
	float mIntensity;
	Vector3 mPosition;
	float mRadius;
	float mDecay;
	float mInner;
	float mOuter;
	float mPad;
};
