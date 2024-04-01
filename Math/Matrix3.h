#pragma once
#include "Vector2.h"
#include "Helper/MyAssert.h"

// 3x3行列
class Matrix3
{
public:
	float m[3][3];

	Matrix3()
		: m()
	{
		*this = Matrix3::kIdentity;
	}

	Matrix3(float m[3][3])
		: m()
	{
		memcpy(this->m, m, sizeof(float) * 9);
	}

	Vector2 GetScale() const
	{
		return Vector2(
			std::sqrt(m[0][0] * m[0][0] + m[0][1] * m[0][1]),
			std::sqrt(m[1][0] * m[1][0] + m[1][1] * m[1][1]));
	}

	float GetRotate() const
	{
		return std::atan2(m[0][1], m[0][0]);
	}

	Vector2 GetTranslate() const
	{
		return Vector2(m[2][0], m[2][1]);
	}

	// Scale
	static Matrix3 CreateScale(const Vector2& scale)
	{
		float tmp[3][3] =
		{
			{ scale.x,0.0f,0.0f },
			{ 0.0f,scale.y,0.0f },
			{ 0.0f,0.0f,1.0f }
		};
		return Matrix3(tmp);
	}

	static Matrix3 CreateScale(float scaleX, float scaleY)
	{
		float tmp[3][3] =
		{
			{ scaleX,0.0f,0.0f },
			{ 0.0f,scaleY,0.0f },
			{ 0.0f,0.0f,1.0f }
		};
		return Matrix3(tmp);
	}

	static Matrix3 CreateScale(float scale)
	{
		float tmp[3][3] =
		{
			{ scale,0.0f,0.0f },
			{ 0.0f,scale,0.0f },
			{ 0.0f,0.0f,1.0f }
		};
		return Matrix3(tmp);
	}

	// Rotate
	static Matrix3 CreateRotate(float rotate)
	{
		float c = std::cos(rotate);
		float s = std::sin(rotate);
		float tmp[3][3] =
		{
			{ c,s,0.0f },
			{ -s,c,0.0f },
			{ 0.0f,0.0f,1.0f }
		};
		return Matrix3(tmp);
	}

	// Translate
	static Matrix3 CreateTranslate(const Vector2& trans)
	{
		float tmp[3][3] =
		{
			{ 1.0f,0.0f,0.0f },
			{ 0.0f,1.0f,0.0f },
			{ trans.x,trans.y,1.0f }
		};
		return Matrix3(tmp);
	}

	static Matrix3 CreateTranslate(float transX, float transY)
	{
		float tmp[3][3] =
		{
			{ 1.0f,0.0f,0.0f },
			{ 0.0f,1.0f,0.0f },
			{ transX,transY,1.0f }
		};
		return Matrix3(tmp);
	}

	// Affine
	static Matrix3 CreateAffine(const Vector2& scale, float rotate, const Vector2& trans)
	{
		Matrix3 result = CreateRotate(rotate);
		result.m[0][0] *= scale.x;
		result.m[1][1] *= scale.y;
		result.m[2][0] = trans.x;
		result.m[2][1] = trans.y;
		return result;
	}

	static const Matrix3 kIdentity;
};

inline Matrix3 operator*(const Matrix3& a, const Matrix3& b)
{
	Matrix3 result = {};
	result.m[0][0] = a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0] + a.m[0][2] * b.m[2][0];
	result.m[0][1] = a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1] + a.m[0][2] * b.m[2][1];
	result.m[0][2] = a.m[0][0] * b.m[0][2] + a.m[0][1] * b.m[1][2] + a.m[0][2] * b.m[2][2];
	result.m[1][0] = a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0] + a.m[1][2] * b.m[2][0];
	result.m[1][1] = a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1] + a.m[1][2] * b.m[2][1];
	result.m[1][2] = a.m[1][0] * b.m[0][2] + a.m[1][1] * b.m[1][2] + a.m[1][2] * b.m[2][2];
	result.m[2][0] = a.m[2][0] * b.m[0][0] + a.m[2][1] * b.m[1][0] + a.m[2][2] * b.m[2][0];
	result.m[2][1] = a.m[2][0] * b.m[0][1] + a.m[2][1] * b.m[1][1] + a.m[2][2] * b.m[2][1];
	result.m[2][2] = a.m[2][0] * b.m[0][2] + a.m[2][1] * b.m[1][2] + a.m[2][2] * b.m[2][2];
	return result;
}

inline Vector2 operator*(const Vector2& a, const Matrix3& b)
{
	return Vector2(
		a.x * b.m[0][0] + a.y * b.m[1][0] + b.m[2][0],
		a.x * b.m[0][1] + a.y * b.m[1][1] + b.m[2][1]);
}

inline Matrix3& operator*=(Matrix3& a, const Matrix3& b)
{
	a = a * b;
	return a;
}

inline Vector2& operator*=(Vector2& a, const Matrix3& b)
{
	a = a * b;
	return a;
}

// 行列式
inline float Determinant(const Matrix3& a)
{
	return
		a.m[0][0] * (a.m[1][1] * a.m[2][2] - a.m[1][2] - a.m[2][1]) +
		a.m[0][1] * (a.m[1][2] * a.m[2][0] - a.m[1][0] - a.m[2][2]) +
		a.m[0][2] * (a.m[1][0] * a.m[2][1] - a.m[1][1] - a.m[2][0]);
}

// 逆行列
inline Matrix3 Inverse(const Matrix3& a)
{
	float det =
		a.m[0][0] * (a.m[1][1] * a.m[2][2] - a.m[1][2] - a.m[2][1]) +
		a.m[0][1] * (a.m[1][2] * a.m[2][0] - a.m[1][0] - a.m[2][2]) +
		a.m[0][2] * (a.m[1][0] * a.m[2][1] - a.m[1][1] - a.m[2][0]);
	MY_ASSERT(std::abs(det) > MyMath::kEpsilon);
	float oneOverDet = 1.0f / det;
	Matrix3 result = {};
	result.m[0][0] = (a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1]) * oneOverDet;
	result.m[0][1] = (a.m[0][2] * a.m[2][1] - a.m[0][1] * a.m[2][2]) * oneOverDet;
	result.m[0][2] = (a.m[0][1] * a.m[1][2] - a.m[0][2] * a.m[1][1]) * oneOverDet;
	result.m[1][0] = (a.m[1][2] * a.m[2][0] - a.m[1][0] * a.m[2][2]) * oneOverDet;
	result.m[1][1] = (a.m[0][0] * a.m[2][2] - a.m[0][2] * a.m[2][0]) * oneOverDet;
	result.m[1][2] = (a.m[0][2] * a.m[1][0] - a.m[0][0] * a.m[1][2]) * oneOverDet;
	result.m[2][0] = (a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0]) * oneOverDet;
	result.m[2][1] = (a.m[0][1] * a.m[2][0] - a.m[0][0] * a.m[2][1]) * oneOverDet;
	result.m[2][2] = (a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0]) * oneOverDet;
	return result;
}

// 転置
Matrix3 Transpose(const Matrix3& a)
{
	Matrix3 result = {};
	result.m[0][0] = a.m[0][0];
	result.m[0][1] = a.m[1][0];
	result.m[0][2] = a.m[2][0];
	result.m[1][0] = a.m[0][1];
	result.m[1][1] = a.m[1][1];
	result.m[1][2] = a.m[2][1];
	result.m[2][0] = a.m[0][2];
	result.m[2][1] = a.m[1][2];
	result.m[2][2] = a.m[2][2];
	return result;
}
