#include "Vector3.h"
#include "Vector2.h"

const Vector3 Vector3::kZero(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::kOne(1.0f, 1.0f, 1.0f);

Vector2 ToVector2(const Vector3 v)
{
	return Vector2(v.x, v.y);
}

Vector3 ToVector3(const Vector2 v)
{
	return Vector3(v.x, v.y, 0.0f);
}
