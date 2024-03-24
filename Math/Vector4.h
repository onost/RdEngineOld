#pragma once
#include "Helper/MyAssert.h"
#include <cmath>

// 4次元ベクトル
class Vector4
{
public:
	float x;
	float y;
	float z;
	float w;

	Vector4()
		: x(0.0f)
		, y(0.0f)
		, z(0.0f)
		, w(0.0f)
	{}

	Vector4(float x, float y, float z, float w)
		: x(x)
		, y(y)
		, z(z)
		, w(w)
	{}

	static const Vector4 kZero;
	static const Vector4 kOne;
};

inline bool operator==(const Vector4& v1, const Vector4& v2)
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
}

inline bool operator!=(const Vector4& v1, const Vector4& v2)
{
	return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z || v1.w != v2.w;
}

inline Vector4 operator-(const Vector4& v)
{
	return Vector4(-v.x, -v.y, -v.z, -v.w);
}

inline Vector4 operator+(const Vector4& v1, const Vector4& v2)
{
	return Vector4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

inline Vector4 operator-(const Vector4& v1, const Vector4& v2)
{
	return Vector4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}

inline Vector4 operator*(const Vector4& v1, const Vector4& v2)
{
	return Vector4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}

inline Vector4 operator*(const Vector4& v, float a)
{
	return Vector4(v.x * a, v.y * a, v.z * a, v.w * a);
}

inline Vector4 operator*(float a, const Vector4& v)
{
	return Vector4(a * v.x, a * v.y, a * v.z, a * v.w);
}

inline Vector4 operator/(const Vector4& v, float a)
{
	float oneOverA = 1.0f / a;
	MY_ASSERT(fabsf(oneOverA) > 0.001f);
	return Vector4(v.x * oneOverA, v.y * oneOverA, v.z * oneOverA, v.w * oneOverA);
}

inline Vector4& operator+=(Vector4& v1, const Vector4& v2)
{
	v1 = v1 + v2;
	return v1;
}

inline Vector4& operator-=(Vector4& v1, const Vector4& v2)
{
	v1 = v1 - v2;
	return v1;
}

inline Vector4& operator*=(Vector4& v1, const Vector4& v2)
{
	v1 = v1 * v2;
	return v1;
}

inline Vector4& operator*=(Vector4& v, float a)
{
	v = v * a;
	return v;
}

inline Vector4& operator/=(Vector4& v, float a)
{
	float oneOverA = 1.0f / a;
	MY_ASSERT(fabsf(oneOverA) > 0.001f);
	v *= oneOverA;
	return v;
}
