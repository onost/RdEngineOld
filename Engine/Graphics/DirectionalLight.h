#pragma once
#include "Color.h"
#include "Vector3.h"

struct DirectionalLight
{
	Color mColor;
	Vector3 mDirection;
	float mIntensity;
};
