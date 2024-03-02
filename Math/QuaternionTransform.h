#pragma once
#include "Quaternion.h"
#include "Vector3.h"

struct QuaternionTransform
{
	Vector3 mScale;
	Quaternion mRotate;
	Vector3 mTranslate;
};
