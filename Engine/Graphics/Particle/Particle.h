#pragma once
#include "Color.h"
#include "Vector3.h"

// パーティクル
struct Particle
{
	float mCurrTime;
	float mLife;
	Vector3 mPosition;
	Vector3 mVel;
	Vector3 mAccel;
	float mScale, mSScale, mEScale;
	Vector3 mRot, mSRot, mERot;
	Color mColor, mSColor, mEColor;
};
