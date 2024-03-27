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

inline bool operator==(const Vector4& a, const Vector4& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

inline bool operator!=(const Vector4& a, const Vector4& b)
{
	return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
}

inline Vector4 operator-(const Vector4& a)
{
	return Vector4(-a.x, -a.y, -a.z, -a.w);
}

inline Vector4 operator+(const Vector4& a, const Vector4& b)
{
	return Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

inline Vector4 operator-(const Vector4& a, const Vector4& b)
{
	return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

inline Vector4 operator*(const Vector4& a, const Vector4& b)
{
	return Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

inline Vector4 operator*(const Vector4& a, float b)
{
	return Vector4(a.x * b, a.y * b, a.z * b, a.w * b);
}

inline Vector4 operator*(float a, const Vector4& b)
{
	return Vector4(a * b.x, a * b.y, a * b.z, a * b.w);
}

inline Vector4 operator/(const Vector4& a, float b)
{
	float oneOverA = 1.0f / b;
	return Vector4(a.x * oneOverA, a.y * oneOverA, a.z * oneOverA, a.w * oneOverA);
}

inline Vector4& operator+=(Vector4& a, const Vector4& b)
{
	a = a + b;
	return a;
}

inline Vector4& operator-=(Vector4& a, const Vector4& b)
{
	a = a - b;
	return a;
}

inline Vector4& operator*=(Vector4& a, const Vector4& b)
{
	a = a * b;
	return a;
}

inline Vector4& operator*=(Vector4& a, float b)
{
	a = a * b;
	return a;
}

inline Vector4& operator/=(Vector4& a, float b)
{
	float oneOverA = 1.0f / b;
	a *= oneOverA;
	return a;
}
