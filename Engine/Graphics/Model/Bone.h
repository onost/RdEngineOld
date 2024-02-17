#pragma once
#include "Matrix4.h"
#include "Quaternion.h"
#include "Vector3.h"
#include <string>

// ボーン用トランスフォーム
struct BoneTransform
{
	Vector3 mScale;
	Quaternion mRotation;
	Vector3 mPosition;

	Matrix4 GetMatrix();
	// 補間
	static BoneTransform Interpolate(
		const BoneTransform& a, const BoneTransform& b, float t);
};

// ボーン
struct Bone
{
	std::string mName;
	BoneTransform mLocal;
	int32_t mParent;
};
