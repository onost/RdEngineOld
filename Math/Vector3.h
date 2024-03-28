#pragma once
#include "MyMath.h"
#include <cmath>

class Vector2;

// 3次元ベクトル
class Vector3
{
public:
	float x;
	float y;
	float z;

	Vector3()
		: x(0.0f)
		, y(0.0f)
		, z(0.0f)
	{}

	Vector3(float x, float y, float z)
		: x(x)
		, y(y)
		, z(z)
	{}

	// ベクトルを正規化
	void Normalize()
	{
		float a = x * x + y * y + z * z;
		if (a >= MyMath::kEpsilon)
		{
			float oneOverA = 1.0f / sqrt(a);
			x *= oneOverA;
			y *= oneOverA;
			z *= oneOverA;
		}
	}

	static const Vector3 kZero;
	static const Vector3 kOne;
};

inline bool operator==(const Vector3& a, const Vector3& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

inline bool operator!=(const Vector3& a, const Vector3& b)
{
	return a.x != b.x || a.y != b.y || a.z != b.z;
}

inline Vector3 operator-(const Vector3& a)
{
	return Vector3(-a.x, -a.y, -a.z);
}

inline Vector3 operator+(const Vector3& a, const Vector3& b)
{
	return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline Vector3 operator-(const Vector3& a, const Vector3& b)
{
	return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline Vector3 operator*(const Vector3& a, const Vector3& b)
{
	return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

inline Vector3 operator*(const Vector3& a, float b)
{
	return Vector3(a.x * b, a.y * b, a.z * b);
}

inline Vector3 operator*(float a, const Vector3& b)
{
	return Vector3(a * b.x, a * b.y, a * b.z);
}

inline Vector3 operator/(const Vector3& a, float b)
{
	float oneOverA = 1.0f / b;
	return Vector3(a.x * oneOverA, a.y * oneOverA, a.z * oneOverA);
}

inline Vector3& operator+=(Vector3& a, const Vector3& b)
{
	a = a + b;
	return a;
}

inline Vector3& operator-=(Vector3& a, const Vector3& b)
{
	a = a - b;
	return a;
}

inline Vector3& operator*=(Vector3& a, const Vector3& b)
{
	a = a * b;
	return a;
}

inline Vector3& operator*=(Vector3& a, float b)
{
	a = a * b;
	return a;
}

inline Vector3& operator/=(Vector3& a, float b)
{
	float oneOverA = 1.0f / b;
	a *= oneOverA;
	return a;
}

// ベクトルの外積
inline Vector3 Cross(const Vector3& a, const Vector3& b)
{
	return Vector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}

// ベクトルの内積
inline float Dot(const Vector3& a, const Vector3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// ベクトルの大きさ
inline float Length(const Vector3& a)
{
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

// ベクトルの大きさの2乗
inline float LengthSq(const Vector3& a)
{
	return a.x * a.x + a.y * a.y + a.z * a.z;
}

// ベクトルを正規化
inline Vector3 Normalize(const Vector3& a)
{
	Vector3 result = a;
	result.Normalize();
	return result;
}

// 2次元ベクトルへ
Vector3 ToVector3(const Vector2 a);
