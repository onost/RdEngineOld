#include "Matrix3.h"
#include "Helper/MyAssert.h"
#include "MyMath.h"
#include "Vector2.h"

static float sIdentity3[3][3] =
{
	1.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,
	0.0f,0.0f,1.0f
};
const Matrix3 Matrix3::kIdentity(sIdentity3);

Matrix3::Matrix3()
{
	*this = Matrix3::kIdentity;
}

Matrix3::Matrix3(float m[3][3])
{
	memcpy(this->m, m, sizeof(float) * 9);
}

Matrix3 Matrix3::CreateScale(const Vector2& scale)
{
	float tmp[3][3] =
	{
		{ scale.x, 0.0f, 0.0f },
		{ 0.0f, scale.y, 0.0f },
		{ 0.0f, 0.0f, 1.0f }
	};
	return Matrix3(tmp);
}

Matrix3 Matrix3::CreateScale(float x, float y)
{
	return CreateScale(Vector2(x, y));
}

Matrix3 Matrix3::CreateScale(float scale)
{
	return CreateScale(Vector2(scale, scale));
}

Matrix3 Matrix3::CreateRotate(float rotate)
{
	float cos = cosf(rotate);
	float sin = sinf(rotate);
	float tmp[3][3] =
	{
		{ cos, sin, 0.0f },
		{ -sin, cos, 0.0f },
		{ 0.0f, 0.0f, 1.0f }
	};
	return Matrix3(tmp);
}

Matrix3 Matrix3::CreateTranslate(const Vector2& trans)
{
	float tmp[3][3] =
	{
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ trans.x, trans.y, 1.0f }
	};
	return Matrix3(tmp);
}

Matrix3 Matrix3::CreateTranslate(float x, float y)
{
	float tmp[3][3] =
	{
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ x, y, 1.0f }
	};
	return Matrix3(tmp);
}

Matrix3 Matrix3::CreateAffine(
	const Vector2& scale, float rotate, const Vector2& trans)
{
	Matrix3 result = CreateRotate(rotate);
	result.m[0][0] *= scale.x;
	result.m[0][1] *= scale.x;
	result.m[1][0] *= scale.y;
	result.m[1][1] *= scale.y;
	result.m[2][0] = trans.x;
	result.m[2][1] = trans.y;
	return result;
}

Vector2 Matrix3::GetTranslation()
{
	return Vector2(m[2][0], m[2][1]);
}

Matrix3 operator*(const Matrix3& m1, const Matrix3& m2)
{
	Matrix3 result;
	result.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0];
	result.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1];
	result.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2];
	result.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0];
	result.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1];
	result.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2];
	result.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0];
	result.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1];
	result.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2];
	return result;
}

Vector2 operator*(const Vector2& v, const Matrix3& m)
{
	return Vector2(
		v.x * m.m[0][0] + v.y * m.m[1][0] + m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + m.m[2][1]);
}

Matrix3 operator*=(Matrix3& m1, const Matrix3& m2)
{
	m1 = m1 * m2;
	return m1;
}

Vector2& operator*=(Vector2& v, const Matrix3& m)
{
	v = v * m;
	return v;
}

float Determinant(const Matrix3& m)
{
	return
		m.m[0][0] * (m.m[1][1] * m.m[2][2] - m.m[1][2] - m.m[2][1]) +
		m.m[0][1] * (m.m[1][2] * m.m[2][0] - m.m[1][0] - m.m[2][2]) +
		m.m[0][2] * (m.m[1][0] * m.m[2][1] - m.m[1][1] - m.m[2][0]);
}

Matrix3 Inverse(const Matrix3& m)
{
	float d =
		m.m[0][0] * (m.m[1][1] * m.m[2][2] - m.m[1][2] - m.m[2][1]) +
		m.m[0][1] * (m.m[1][2] * m.m[2][0] - m.m[1][0] - m.m[2][2]) +
		m.m[0][2] * (m.m[1][0] * m.m[2][1] - m.m[1][1] - m.m[2][0]);
	MY_ASSERT(fabsf(d) > MyMath::kEpsilon);
	float oneOverD = 1.0f / d;
	Matrix3 result;
	result.m[0][0] = (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1]) * oneOverD;
	result.m[0][1] = (m.m[0][2] * m.m[2][1] - m.m[0][1] * m.m[2][2]) * oneOverD;
	result.m[0][2] = (m.m[0][1] * m.m[1][2] - m.m[0][2] * m.m[1][1]) * oneOverD;
	result.m[1][0] = (m.m[1][2] * m.m[2][0] - m.m[1][0] * m.m[2][2]) * oneOverD;
	result.m[1][1] = (m.m[0][0] * m.m[2][2] - m.m[0][2] * m.m[2][0]) * oneOverD;
	result.m[1][2] = (m.m[0][2] * m.m[1][0] - m.m[0][0] * m.m[1][2]) * oneOverD;
	result.m[2][0] = (m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0]) * oneOverD;
	result.m[2][1] = (m.m[0][1] * m.m[2][0] - m.m[0][0] * m.m[2][1]) * oneOverD;
	result.m[2][2] = (m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0]) * oneOverD;
	return result;
}

Matrix3 Transpose(const Matrix3& m)
{
	Matrix3 result;
	result.m[0][0] = m.m[0][0];
	result.m[0][1] = m.m[1][0];
	result.m[0][2] = m.m[2][0];
	result.m[1][0] = m.m[0][1];
	result.m[1][1] = m.m[1][1];
	result.m[1][2] = m.m[2][1];
	result.m[2][0] = m.m[0][2];
	result.m[2][1] = m.m[1][2];
	result.m[2][2] = m.m[2][2];
	return result;
}
