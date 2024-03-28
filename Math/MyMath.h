#pragma once
#include <limits>
#include <numbers>

namespace MyMath
{
	const float kPi = std::numbers::pi_v<float>;// π
	const float k2Pi = kPi * 2.0f;// 2π
	const float kPiOver2 = kPi / 2.0f;// π/2
	const float k1OverPi = 1.0f / kPi;// 1/π
	const float k1Over2Pi = 1.0f / k2Pi;// 1/2π
	const float kInfinity = std::numeric_limits<float>::infinity();// ∞
	const float kEpsilon = std::numeric_limits<float>::epsilon();

	template <typename T>
	inline T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	template <typename T>
	inline T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template <typename T>
	inline T Clamp(const T& value, const T& min, const T& max)
	{
		return Min(max, Max(min, value));
	}

	template <typename T>
	inline T Lerp(const T& a, const T& b, float t)
	{
		return (1.0f - t) * a + t * b;
	}

	inline float ToDegrees(float a)
	{
		return a * 180.0f / kPi;
	}

	inline float ToRadians(float a)
	{
		return a * kPi / 180.0f;
	}

	inline int Sign(float a)
	{
		return (a > 0.0f) - (a < 0.0f);
	}
}
