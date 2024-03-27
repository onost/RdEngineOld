#include "Vector2.h"
#include "Vector3.h"

const Vector2 Vector2::kZero(0.0f, 0.0f);
const Vector2 Vector2::kOne(1.0f, 1.0f);

// 3次元ベクトルへ
Vector2 ToVector2(const Vector3 a)
{
	return Vector2(a.x, a.y);
}
