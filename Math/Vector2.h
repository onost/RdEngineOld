#pragma once
#include "MyMath.h"
#include <cmath>

// 2次元ベクトル
class Vector2
{
public:
	float x;
	float y;

	Vector2()
		: x(0.0f)
		, y(0.0f)
	{}

	Vector2(float x, float y)
		: x(x)
		, y(y)
	{}

	void Normalize()
	{
		float a = x * x + y * y;
		if (a > MyMath::kEpsilon)
		{
			float oneOverA = 1.0f / sqrt(a);
			x *= oneOverA;
			y *= oneOverA;
		}
	}

	static const Vector2 kZero;
	static const Vector2 kOne;
};

inline bool operator==(const Vector2& v1, const Vector2& v2)
{
	return v1.x == v2.x && v1.y == v2.y;
}

inline bool operator!=(const Vector2& v1, const Vector2& v2)
{
	return v1.x != v2.x || v1.y != v2.y;
}

inline Vector2 operator-(const Vector2& v)
{
	return Vector2(-v.x, -v.y);
}

inline Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
	return Vector2(v1.x + v2.x, v1.y + v2.y);
}

inline Vector2 operator-(const Vector2& v1, const Vector2& v2)
{
	return Vector2(v1.x - v2.x, v1.y - v2.y);
}

inline Vector2 operator*(const Vector2& v1, const Vector2& v2)
{
	return Vector2(v1.x * v2.x, v1.y * v2.y);
}

inline Vector2 operator*(const Vector2& v, float a)
{
	return Vector2(v.x * a, v.y * a);
}

inline Vector2 operator*(float a, const Vector2& v)
{
	return Vector2(a * v.x, a * v.y);
}

inline Vector2 operator/(const Vector2& v, float a)
{
	float oneOverA = 1.0f / a;
	return Vector2(v.x * oneOverA, v.y * oneOverA);
}

inline Vector2& operator+=(Vector2& v1, const Vector2& v2)
{
	v1 = v1 + v2;
	return v1;
}

inline Vector2& operator-=(Vector2& v1, const Vector2& v2)
{
	v1 = v1 - v2;
	return v1;
}

inline Vector2& operator*=(Vector2& v1, const Vector2& v2)
{
	v1 = v1 * v2;
	return v1;
}

inline Vector2& operator*=(Vector2& v, float a)
{
	v = v * a;
	return v;
}

inline Vector2& operator/=(Vector2& v, float a)
{
	float oneOverA = 1.0f / a;
	v *= oneOverA;
	return v;
}

inline float Cross(const Vector2& v1, const Vector2& v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

inline float Dot(const Vector2& v1, const Vector2& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

inline float Length(const Vector2& v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

inline float LengthSq(const Vector2& v)
{
	return v.x * v.x + v.y * v.y;
}

inline Vector2 Normalize(const Vector2& v)
{
	Vector2 result = v;
	result.Normalize();
	return result;
}
