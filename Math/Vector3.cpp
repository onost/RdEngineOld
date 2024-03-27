#include "Vector3.h"
#include "Vector2.h"

const Vector3 Vector3::kZero(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::kOne(1.0f, 1.0f, 1.0f);

// 2次元ベクトルへ
Vector3 ToVector3(const Vector2 a)
{
	return Vector3(a.x, a.y, 0.0f);
}
