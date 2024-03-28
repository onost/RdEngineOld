#pragma once
#include "MyMath.h"
#include <cmath>

class Vector3;

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

	// ベクトルを正規化
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

inline bool operator==(const Vector2& a, const Vector2& b)
{
	return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const Vector2& a, const Vector2& b)
{
	return a.x != b.x || a.y != b.y;
}

inline Vector2 operator-(const Vector2& a)
{
	return Vector2(-a.x, -a.y);
}

inline Vector2 operator+(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x + b.x, a.y + b.y);
}

inline Vector2 operator-(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x - b.x, a.y - b.y);
}

inline Vector2 operator*(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x * b.x, a.y * b.y);
}

inline Vector2 operator*(const Vector2& a, float b)
{
	return Vector2(a.x * b, a.y * b);
}

inline Vector2 operator*(float a, const Vector2& b)
{
	return Vector2(a * b.x, a * b.y);
}

inline Vector2 operator/(const Vector2& a, float b)
{
	float oneOverA = 1.0f / b;
	return Vector2(a.x * oneOverA, a.y * oneOverA);
}

inline Vector2& operator+=(Vector2& a, const Vector2& b)
{
	a = a + b;
	return a;
}

inline Vector2& operator-=(Vector2& a, const Vector2& b)
{
	a = a - b;
	return a;
}

inline Vector2& operator*=(Vector2& a, const Vector2& b)
{
	a = a * b;
	return a;
}

inline Vector2& operator*=(Vector2& a, float b)
{
	a = a * b;
	return a;
}

inline Vector2& operator/=(Vector2& a, float b)
{
	float oneOverA = 1.0f / b;
	a *= oneOverA;
	return a;
}

// ベクトルの外積
inline float Cross(const Vector2& a, const Vector2& b)
{
	return a.x * b.y - a.y * b.x;
}

// ベクトルの内積
inline float Dot(const Vector2& a, const Vector2& b)
{
	return a.x * b.x + a.y * b.y;
}

// ベクトルの大きさ
inline float Length(const Vector2& a)
{
	return sqrt(a.x * a.x + a.y * a.y);
}

// ベクトルの大きさの2乗
inline float LengthSq(const Vector2& a)
{
	return a.x * a.x + a.y * a.y;
}

// ベクトルを正規化
inline Vector2 Normalize(const Vector2& a)
{
	Vector2 result = a;
	result.Normalize();
	return result;
}

// 3次元ベクトルへ
Vector2 ToVector2(const Vector3 a);
