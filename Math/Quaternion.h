#pragma once
#include "Vector3.h"
#include "Helper/MyAssert.h"

// 四元数
class Quaternion
{
public:
	float w;
	float x;
	float y;
	float z;

	Quaternion()
		: w(0.0f)
		, x(0.0f)
		, y(0.0f)
		, z(0.0f)
	{
		*this = Quaternion::kIdentity;
	}

	Quaternion(float w, float x, float y, float z)
		: w(w)
		, x(x)
		, y(y)
		, z(z)
	{}

	Quaternion(const Vector3& axis, float theta)
	{
		MY_ASSERT(std::abs(Length(axis) - 1.0f) < 0.001f);

		float tOver2 = theta * 0.5f;
		float s = std::sin(tOver2);
		w = std::cos(tOver2);
		x = axis.x * s;
		y = axis.y * s;
		z = axis.z * s;
	}

	// 四元数を正規化
	void Normalize()
	{
		float a = w * w + x * x + y * y + z * z;
		if (a >= MyMath::kEpsilon)
		{
			float oneOverA = 1.0f / std::sqrt(a);
			w *= oneOverA;
			x *= oneOverA;
			y *= oneOverA;
			z *= oneOverA;
		}
	}

	Vector3 GetAxis()
	{
		float sinSq = 1.0f - w * w;
		if (sinSq <= 0.0f)
		{
			return Vector3(1.0f, 0.0f, 0.0f);
		}
		float oneOverSinSq = 1.0f / std::sqrt(sinSq);
		return Vector3(x * oneOverSinSq, y * oneOverSinSq, z * oneOverSinSq);
	}

	float GetTheta()
	{
		return std::acos(w) * 2.0f;
	}

	static const Quaternion kIdentity;
};

inline Quaternion operator*(const Quaternion& a, const Quaternion& b)
{
	Quaternion result = {};
	result.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
	result.x = a.w * b.x + a.x * b.w + a.z * b.y - a.y * b.z;
	result.y = a.w * b.y + a.y * b.w + a.x * b.z - a.z * b.x;
	result.z = a.w * b.z + a.z * b.w + a.y * b.x - a.x * b.y;
	return result;
}

// 四元数の共役
inline Quaternion Conjugate(const Quaternion& a)
{
	Quaternion result = {};
	result.w = a.w;
	result.x = -a.x;
	result.y = -a.y;
	result.z = -a.z;
	return result;
}

inline Vector3 operator*(const Vector3& a, const Quaternion& b)
{
	Quaternion result = {};
	result = Conjugate(b) * Quaternion(0.0f, a.x, a.y, a.z) * b;
	return Vector3(result.x, result.y, result.z);
}

inline Quaternion& operator*=(Quaternion& a, const Quaternion& b)
{
	a = a * b;
	return a;
}

inline Vector3& operator*=(Vector3& a, const Quaternion& b)
{
	a = a * b;
	return a;
}

// 四元数の内積
inline float Dot(const Quaternion& a, const Quaternion& b)
{
	return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
}

// 四元数の線形補間
inline Quaternion Lerp(const Quaternion& a, const Quaternion& b, float t)
{
	Quaternion result = {};
	result.w = std::lerp(a.w, b.w, t);
	result.x = std::lerp(a.x, b.x, t);
	result.y = std::lerp(a.y, b.y, t);
	result.z = std::lerp(a.z, b.z, t);
	result.Normalize();
	return result;
}

// 四元数を正規化
inline Quaternion Normalize(const Quaternion& a)
{
	Quaternion result = a;
	result.Normalize();
	return result;
}

// 四元数の累乗
inline Quaternion Pow(const Quaternion& a, float b)
{
	if (std::abs(a.w) > 1.0f - MyMath::kEpsilon)
	{
		return a;
	}

	float alpha = std::cos(a.w);
	float newAlpha = alpha * b;
	Quaternion result = {};
	result.w = std::cos(newAlpha);
	newAlpha = std::sin(newAlpha) / std::sin(alpha);
	result.x = a.x * newAlpha;
	result.y = a.y * newAlpha;
	result.z = a.z * newAlpha;
	return result;
}

// 四元数の球面線形補間
inline Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t)
{
	if (t <= 0.0f)
	{
		return a;
	}
	if (t >= 1.0f)
	{
		return b;
	}
	float cosOmega = Dot(a, b);
	float w = b.w;
	float x = b.x;
	float y = b.y;
	float z = b.z;
	if (cosOmega < 0.0f)
	{
		w = -b.w;
		x = -b.x;
		y = -b.y;
		z = -b.z;
		cosOmega = -cosOmega;
	}
	float k0, k1;
	if (cosOmega > 1.0f - MyMath::kEpsilon)
	{
		k0 = 1.0f - t;
		k1 = t;
	}
	else
	{
		float sinOmega = std::sqrt(1.0f - cosOmega * cosOmega);
		float omega = std::atan2(sinOmega, cosOmega);
		float oneOverSin = 1.0f / sinOmega;
		k0 = std::sin((1.0f - t) * omega) * oneOverSin;
		k1 = std::sin(t * omega) * oneOverSin;
	}
	Quaternion result = {};
	result.w = k0 * a.w + k1 * w;
	result.x = k0 * a.x + k1 * x;
	result.y = k0 * a.y + k1 * y;
	result.z = k0 * a.z + k1 * z;
	return result;
}
