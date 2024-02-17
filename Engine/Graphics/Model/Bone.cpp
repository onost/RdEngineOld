#include "Bone.h"
#include "MyMath.h"

Matrix4 BoneTransform::GetMatrix()
{
	return Matrix4::CreateAffine(mScale, mRotation, mPosition);
}

// 補間
BoneTransform BoneTransform::Interpolate(
	const BoneTransform& a, const BoneTransform& b, float t)
{
	BoneTransform result;
	result.mScale = MyMath::Lerp<Vector3>(a.mScale, b.mScale, t);
	result.mRotation = Slerp(a.mRotation, b.mRotation, t);
	result.mPosition = MyMath::Lerp<Vector3>(a.mPosition, b.mPosition, t);
	return result;
}
