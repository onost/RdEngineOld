#include "Matrix4.h"
#include "Helper/MyAssert.h"
#include "MyMath.h"
#include "Quaternion.h"
#include "Vector3.h"

static float sIdentity4[4][4] =
{
	1.0f,0.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,0.0f,
	0.0f,0.0f,1.0f,0.0f,
	0.0f,0.0f,0.0f,1.0f
};
const Matrix4 Matrix4::kIdentity(sIdentity4);

Matrix4::Matrix4()
{
	*this = Matrix4::kIdentity;
}

Matrix4::Matrix4(float m[4][4])
{
	memcpy(this->m, m, sizeof(float) * 16);
}

Matrix4 Matrix4::CreateScale(const Vector3& scale)
{
	float tmp[4][4] =
	{
		{ scale.x, 0.0f, 0.0f, 0.0f },
		{ 0.0f, scale.y, 0.0f, 0.0f },
		{ 0.0f, 0.0f, scale.z, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
	return Matrix4(tmp);
}

Matrix4 Matrix4::CreateScale(float x, float y, float z)
{
	return CreateScale(Vector3(x, y, z));
}

Matrix4 Matrix4::CreateScale(float scale)
{
	return CreateScale(Vector3(scale, scale, scale));
}

Matrix4 Matrix4::CreateRotateX(float rotate)
{
	float cos = cosf(rotate);
	float sin = sinf(rotate);
	float tmp[4][4] =
	{
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, cos, sin, 0.0f },
		{ 0.0f, -sin, cos, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
	return Matrix4(tmp);
}

Matrix4 Matrix4::CreateRotateY(float rotate)
{
	float cos = cosf(rotate);
	float sin = sinf(rotate);
	float tmp[4][4] =
	{
		{ cos, 0.0f, -sin, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ sin, 0.0f, cos, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
	return Matrix4(tmp);
}

Matrix4 Matrix4::CreateRotateZ(float rotate)
{
	float cos = cosf(rotate);
	float sin = sinf(rotate);
	float tmp[4][4] =
	{
		{ cos, sin, 0.0f, 0.0f },
		{ -sin, cos, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
	return Matrix4(tmp);
}

Matrix4 Matrix4::CreateRotate(const Quaternion& q)
{
	float ww = 2.0f * q.w;
	float xx = 2.0f * q.x;
	float yy = 2.0f * q.y;
	float zz = 2.0f * q.z;
	Matrix4 result = Matrix4::kIdentity;
	result.m[0][0] = 1.0f - yy * q.y - zz * q.z;
	result.m[0][1] = xx * q.y + ww * q.z;
	result.m[0][2] = xx * q.z - ww * q.y;
	result.m[1][0] = xx * q.y - ww * q.z;
	result.m[1][1] = 1.0f - xx * q.x - zz * q.z;
	result.m[1][2] = yy * q.z + ww * q.x;
	result.m[2][0] = xx * q.z + ww * q.y;
	result.m[2][1] = yy * q.z - ww * q.x;
	result.m[2][2] = 1.0f - xx * q.x - yy * q.y;
	return result;
}

Matrix4 Matrix4::CreateTranslate(const Vector3& trans)
{
	float tmp[4][4] =
	{
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ trans.x, trans.y, trans.z, 1.0f }
	};
	return Matrix4(tmp);
}

Matrix4 Matrix4::CreateAffine(
	const Vector3& scale, const Quaternion& rotate, const Vector3& trans)
{
	Matrix4 result = CreateRotate(rotate);
	result.m[0][0] *= scale.x;
	result.m[0][1] *= scale.x;
	result.m[0][2] *= scale.x;
	result.m[1][0] *= scale.y;
	result.m[1][1] *= scale.y;
	result.m[1][2] *= scale.y;
	result.m[2][0] *= scale.z;
	result.m[2][1] *= scale.z;
	result.m[2][2] *= scale.z;
	result.m[3][0] = trans.x;
	result.m[3][1] = trans.y;
	result.m[3][2] = trans.z;
	return result;
}

Matrix4 Matrix4::CreateTranslate(float x, float y, float z)
{
	return CreateTranslate(Vector3(x, y, z));
}

Matrix4 Matrix4::CreateLookAt(
	const Vector3& eye, const Vector3& target, const Vector3& up)
{
	Vector3 axisZ = Normalize(target - eye);
	Vector3 axisX = Normalize(Cross(up, axisZ));
	Vector3 axisY = Normalize(Cross(axisZ, axisX));
	Vector3 trans = Vector3(-Dot(axisX, eye), -Dot(axisY, eye), -Dot(axisZ, eye));
	float tmp[4][4] =
	{
		{ axisX.x, axisY.x, axisZ.x, 0.0f },
		{ axisX.y, axisY.y, axisZ.y, 0.0f },
		{ axisX.z, axisY.z, axisZ.z, 0.0f },
		{ trans.x, trans.y, trans.z, 1.0f }
	};
	return Matrix4(tmp);
}

Matrix4 Matrix4::CreateOrthographic(
	float left, float top, float right, float bottom, float nearZ, float farZ)
{
	float tmp[4][4] =
	{
		{ 2.0f / (right - left), 0.0f, 0.0f, 0.0f },
		{ 0.0f, 2.0f / (top - bottom), 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f / (farZ - nearZ), 0.0f },
		{ (left + right) / (left - right), (top + bottom) / (bottom - top), nearZ / (nearZ - farZ), 1.0f }
	};
	return Matrix4(tmp);
}

Matrix4 Matrix4::CreatePerspectiveFov(
	float fovY, float aspectRatio, float nearZ, float farZ)
{
	float yScale = 1.0f / tanf(fovY / 2.0f);
	float xScale = yScale / aspectRatio;
	float tmp[4][4] =
	{
		{ xScale, 0.0f, 0.0f, 0.0f },
		{ 0.0f, yScale, 0.0f, 0.0f },
		{ 0.0f, 0.0f, farZ / (farZ - nearZ), 1.0f },
		{ 0.0f, 0.0f, -(nearZ * farZ) / (farZ - nearZ), 0.0f }
	};
	return Matrix4(tmp);
}

Vector3 Matrix4::GetScale() const
{
	return Vector3(
		sqrtf(m[0][0] * m[0][0] + m[0][1] * m[0][1] + m[0][2] * m[0][2]),
		sqrtf(m[1][0] * m[1][0] + m[1][1] * m[1][1] + m[1][2] * m[1][2]),
		sqrtf(m[2][0] * m[2][0] + m[2][1] * m[2][1] + m[2][2] * m[2][2]));
}

Quaternion Matrix4::GetRotation() const
{
	/*Vector3 x = Normalize(Vector3(m[0][0], m[0][1], m[0][2]));
	Vector3 y = Normalize(Vector3(m[1][0], m[1][1], m[1][2]));
	Vector3 z = Normalize(Vector3(m[2][0], m[2][1], m[2][2]));
	float tmp[4][4] =
	{
		{ x.x, x.y, x.z, 0.0f },
		{ y.x, y.y, y.z, 0.0f },
		{ z.x, z.y, z.z, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
	return ToQuaternion(Matrix4(tmp));*/
	Vector3 x = Normalize(Vector3(m[0][0], m[1][0], m[2][0]));
	Vector3 y = Normalize(Vector3(m[0][1], m[1][1], m[2][1]));
	Vector3 z = Normalize(Vector3(m[0][2], m[1][2], m[2][2]));
	float tmp[4][4] =
	{
		{ x.x, y.x, z.x, 0.0f },
		{ x.y, y.y, z.y, 0.0f },
		{ x.z, y.z, z.z, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
	return ToQuaternion(Matrix4(tmp));
}

Quaternion Matrix4::GetRotation(const Vector3& scale) const
{
	/*Vector3 x = Normalize(Vector3(m[0][0], m[0][1], m[0][2]));
	Vector3 y = Normalize(Vector3(m[1][0], m[1][1], m[1][2]));
	Vector3 z = Normalize(Vector3(m[2][0], m[2][1], m[2][2]));
	float tmp[4][4] =
	{
		{ x.x, x.y, x.z, 0.0f },
		{ y.x, y.y, y.z, 0.0f },
		{ z.x, z.y, z.z, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
	return ToQuaternion(Matrix4(tmp));*/
	Vector3 x = Vector3(m[0][0] / scale.x, m[1][0] / scale.x, m[2][0] / scale.x);
	Vector3 y = Vector3(m[0][1] / scale.y, m[1][1] / scale.y, m[2][1] / scale.y);
	Vector3 z = Vector3(m[0][2] / scale.z, m[1][2] / scale.z, m[2][2] / scale.z);
	float tmp[4][4] =
	{
		{ x.x, y.x, z.x, 0.0f },
		{ x.y, y.y, z.y, 0.0f },
		{ x.z, y.z, z.z, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};
	return ToQuaternion(Matrix4(tmp));
}

Vector3 Matrix4::GetTranslation() const
{
	return Vector3(m[3][0], m[3][1], m[3][2]);
}

inline Matrix4 operator*(const Matrix4& m1, const Matrix4& m2)
{
	Matrix4 result;
	result.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	result.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	result.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	result.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];
	result.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	result.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	result.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	result.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];
	result.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	result.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	result.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	result.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];
	result.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	result.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	result.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	result.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
	return result;
}

Vector3 operator*(const Vector3& v, const Matrix4& m)
{
	return Vector3(
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]);
}

Matrix4 operator*=(Matrix4& m1, const Matrix4& m2)
{
	m1 = m1 * m2;
	return m1;
}

Vector3& operator*=(Vector3& v, const Matrix4& m)
{
	v = v * m;
	return v;
}

float Determinant(const Matrix4& m)
{
	return
		m.m[0][0] * ((m.m[1][1] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2])) + (m.m[1][2] * (m.m[2][3] * m.m[3][1] - m.m[2][1] * m.m[3][3])) + (m.m[1][3] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1]))) -
		m.m[0][1] * ((m.m[1][0] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2])) + (m.m[1][2] * (m.m[2][3] * m.m[3][0] - m.m[2][0] * m.m[3][3])) + (m.m[1][3] * (m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0]))) +
		m.m[0][2] * ((m.m[1][0] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1])) + (m.m[1][1] * (m.m[2][3] * m.m[3][0] - m.m[2][0] * m.m[3][3])) + (m.m[1][3] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0]))) -
		m.m[0][3] * ((m.m[1][0] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1])) + (m.m[1][1] * (m.m[2][2] * m.m[3][0] - m.m[2][0] * m.m[3][2])) + (m.m[1][2] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0])));
}

Matrix4 Inverse(const Matrix4& m)
{
	float d =
		m.m[0][0] * ((m.m[1][1] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2])) + (m.m[1][2] * (m.m[2][3] * m.m[3][1] - m.m[2][1] * m.m[3][3])) + (m.m[1][3] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1]))) -
		m.m[0][1] * ((m.m[1][0] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2])) + (m.m[1][2] * (m.m[2][3] * m.m[3][0] - m.m[2][0] * m.m[3][3])) + (m.m[1][3] * (m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0]))) +
		m.m[0][2] * ((m.m[1][0] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1])) + (m.m[1][1] * (m.m[2][3] * m.m[3][0] - m.m[2][0] * m.m[3][3])) + (m.m[1][3] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0]))) -
		m.m[0][3] * ((m.m[1][0] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1])) + (m.m[1][1] * (m.m[2][2] * m.m[3][0] - m.m[2][0] * m.m[3][2])) + (m.m[1][2] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0])));
	MY_ASSERT(fabsf(d) > MyMath::kEpsilon);
	float oneOverD = 1.0f / d;
	Matrix4 result;
	result.m[0][0] = (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1]) * oneOverD;
	result.m[0][1] = (m.m[0][2] * m.m[2][1] - m.m[0][1] * m.m[2][2]) * oneOverD;
	result.m[0][2] = (m.m[0][1] * m.m[1][2] - m.m[0][2] * m.m[1][1]) * oneOverD;
	result.m[0][3] = 0.0f;
	result.m[1][0] = (m.m[1][2] * m.m[2][0] - m.m[1][0] * m.m[2][2]) * oneOverD;
	result.m[1][1] = (m.m[0][0] * m.m[2][2] - m.m[0][2] * m.m[2][0]) * oneOverD;
	result.m[1][2] = (m.m[0][2] * m.m[1][0] - m.m[0][0] * m.m[1][2]) * oneOverD;
	result.m[1][3] = 0.0f;
	result.m[2][0] = (m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0]) * oneOverD;
	result.m[2][1] = (m.m[0][1] * m.m[2][0] - m.m[0][0] * m.m[2][1]) * oneOverD;
	result.m[2][2] = (m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0]) * oneOverD;
	result.m[2][3] = 0.0f;
	result.m[3][0] = -(m.m[3][0] * result.m[0][0] + m.m[3][1] * result.m[1][0] + m.m[3][2] * result.m[2][0]);
	result.m[3][1] = -(m.m[3][0] * result.m[0][1] + m.m[3][1] * result.m[1][1] + m.m[3][2] * result.m[2][1]);
	result.m[3][2] = -(m.m[3][0] * result.m[0][2] + m.m[3][1] * result.m[1][2] + m.m[3][2] * result.m[2][2]);
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4 Transpose(const Matrix4& m)
{
	Matrix4 result;
	result.m[0][0] = m.m[0][0];
	result.m[0][1] = m.m[1][0];
	result.m[0][2] = m.m[2][0];
	result.m[0][3] = m.m[3][0];
	result.m[1][0] = m.m[0][1];
	result.m[1][1] = m.m[1][1];
	result.m[1][2] = m.m[2][1];
	result.m[1][3] = m.m[3][1];
	result.m[2][0] = m.m[0][2];
	result.m[2][1] = m.m[1][2];
	result.m[2][2] = m.m[2][2];
	result.m[2][3] = m.m[3][2];
	result.m[3][0] = m.m[0][3];
	result.m[3][1] = m.m[1][3];
	result.m[3][2] = m.m[2][3];
	result.m[3][3] = m.m[3][3];
	return result;
}
