#pragma once
#include "Quaternion.h"
#include "Vector3.h"
#include "Helper/MyAssert.h"

// 4x4行列
class Matrix4
{
public:
	float m[4][4];

	Matrix4()
		: m()
	{
		*this = Matrix4::kIdentity;
	}

	Matrix4(float m[4][4])
		: m()
	{
		memcpy(this->m, m, sizeof(float) * 16);
	}

	Vector3 GetScale() const
	{
		return Vector3(
			std::sqrt(m[0][0] * m[0][0] + m[0][1] * m[0][1] + m[0][2] * m[0][2]),
			std::sqrt(m[1][0] * m[1][0] + m[1][1] * m[1][1] + m[1][2] * m[1][2]),
			std::sqrt(m[2][0] * m[2][0] + m[2][1] * m[2][1] + m[2][2] * m[2][2]));
	}

	Quaternion GetRotate() const
	{
		Vector3 x = Normalize(Vector3(m[0][0], m[1][0], m[2][0]));
		Vector3 y = Normalize(Vector3(m[0][1], m[1][1], m[2][1]));
		Vector3 z = Normalize(Vector3(m[0][2], m[1][2], m[2][2]));
		float tmp[4][4] =
		{
			{ x.x,y.x,z.x,0.0f },
			{ x.y,y.y,z.y,0.0f },
			{ x.z,y.z,z.z,0.0f },
			{ 0.0f,0.0f,0.0f,1.0f }
		};
		return ToQuaternion(Matrix4(tmp));
	}

	Vector3 GetTranslate() const
	{
		return Vector3(m[3][0], m[3][1], m[3][2]);
	}

	// Scale
	static Matrix4 CreateScale(const Vector3& scale)
	{
		float tmp[4][4] =
		{
			{ scale.x,0.0f,0.0f,0.0f },
			{ 0.0f,scale.y,0.0f,0.0f },
			{ 0.0f,0.0f,scale.z,0.0f },
			{ 0.0f,0.0f,0.0f,1.0f }
		};
		return Matrix4(tmp);
	}

	static Matrix4 CreateScale(float scaleX, float scaleY, float scaleZ)
	{
		float tmp[4][4] =
		{
			{ scaleX,0.0f,0.0f,0.0f },
			{ 0.0f,scaleY,0.0f,0.0f },
			{ 0.0f,0.0f,scaleZ,0.0f },
			{ 0.0f,0.0f,0.0f,1.0f }
		};
		return Matrix4(tmp);
	}

	static Matrix4 CreateScale(float scale)
	{
		float tmp[4][4] =
		{
			{ scale,0.0f,0.0f,0.0f },
			{ 0.0f,scale,0.0f,0.0f },
			{ 0.0f,0.0f,scale,0.0f },
			{ 0.0f,0.0f,0.0f,1.0f }
		};
		return Matrix4(tmp);
	}

	// Rotate
	static Matrix4 CreateRotateX(float rotate)
	{
		float c = std::cos(rotate);
		float s = std::sin(rotate);
		float tmp[4][4] =
		{
			{ 1.0f,0.0f,0.0f,0.0f },
			{ 0.0f,c,s,0.0f },
			{ 0.0f,-s,c,0.0f },
			{ 0.0f,0.0f,0.0f,1.0f }
		};
		return Matrix4(tmp);
	}

	static Matrix4 CreateRotateY(float rotate)
	{
		float c = std::cos(rotate);
		float s = std::sin(rotate);
		float tmp[4][4] =
		{
			{ c,0.0f,-s,0.0f },
			{ 0.0f,1.0f,0.0f,0.0f },
			{ s,0.0f,c,0.0f },
			{ 0.0f,0.0f,0.0f,1.0f }
		};
		return Matrix4(tmp);
	}

	static Matrix4 CreateRotateZ(float rotate)
	{
		float c = std::cos(rotate);
		float s = std::sin(rotate);
		float tmp[4][4] =
		{
			{ c,s,0.0f,0.0f },
			{ -s,c,0.0f,0.0f },
			{ 0.0f,0.0f,1.0f,0.0f },
			{ 0.0f,0.0f,0.0f,1.0f }
		};
		return Matrix4(tmp);
	}

	static Matrix4 CreateRotate(const Quaternion& quat)
	{
		float ww = quat.w * 2.0f;
		float xx = quat.x * 2.0f;
		float yy = quat.y * 2.0f;
		float zz = quat.z * 2.0f;
		Matrix4 result = Matrix4::kIdentity;
		result.m[0][0] = 1.0f - yy * quat.y - zz * quat.z;
		result.m[0][1] = xx * quat.y + ww * quat.z;
		result.m[0][2] = xx * quat.z - ww * quat.y;
		result.m[1][0] = xx * quat.y - ww * quat.z;
		result.m[1][1] = 1.0f - xx * quat.x - zz * quat.z;
		result.m[1][2] = yy * quat.z + ww * quat.x;
		result.m[2][0] = xx * quat.z + ww * quat.y;
		result.m[2][1] = yy * quat.z - ww * quat.x;
		result.m[2][2] = 1.0f - xx * quat.x - yy * quat.y;
		return result;
	}

	// Translate
	static Matrix4 CreateTranslate(const Vector3& trans)
	{
		float tmp[4][4] =
		{
			{ 1.0f,0.0f,0.0f,0.0f },
			{ 0.0f,1.0f,0.0f,0.0f },
			{ 0.0f,0.0f,1.0f,0.0f },
			{ trans.x,trans.y,trans.z,1.0f }
		};
		return Matrix4(tmp);
	}

	static Matrix4 CreateTranslate(float transX, float transY, float transZ)
	{
		float tmp[4][4] =
		{
			{ 1.0f,0.0f,0.0f,0.0f },
			{ 0.0f,1.0f,0.0f,0.0f },
			{ 0.0f,0.0f,1.0f,0.0f },
			{ transX,transY,transZ,1.0f }
		};
		return Matrix4(tmp);
	}

	// Affine
	static Matrix4 CreateAffine(const Vector3& scale, const Quaternion& rotate, const Vector3& trans)
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

	// Look At
	static Matrix4 CreateLookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
	{
		Vector3 z = Normalize(target - eye);
		Vector3 x = Normalize(Cross(up, z));
		Vector3 y = Normalize(Cross(z, x));
		Vector3 trans = Vector3(-Dot(x, eye), -Dot(y, eye), -Dot(z, eye));
		float tmp[4][4] =
		{
			{ x.x,y.x,z.x,0.0f },
			{ x.y,y.y,z.y,0.0f },
			{ x.z,y.z,z.z,0.0f },
			{ trans.x,trans.y,trans.z,1.0f }
		};
		return Matrix4(tmp);
	}

	// Orthographic
	static Matrix4 CreateOrthographic(float left, float top, float right, float bottom, float nearZ, float farZ)
	{
		float tmp[4][4] =
		{
			{ 2.0f / (right - left),0.0f,0.0f,0.0f },
			{ 0.0f,2.0f / (top - bottom),0.0f,0.0f },
			{ 0.0f,0.0f,1.0f / (farZ - nearZ),0.0f },
			{ (left + right) / (left - right),(top + bottom) / (bottom - top),nearZ / (nearZ - farZ),1.0f }
		};
		return Matrix4(tmp);
	}

	// Perspective
	static Matrix4 CreatePerspectiveFov(float fovY, float aspectRatio, float nearZ, float farZ)
	{
		float yScale = 1.0f / std::tan(fovY / 2.0f);
		float xScale = yScale / aspectRatio;
		float tmp[4][4] =
		{
			{ xScale,0.0f,0.0f,0.0f },
			{ 0.0f,yScale,0.0f,0.0f },
			{ 0.0f,0.0f,farZ / (farZ - nearZ),1.0f },
			{ 0.0f,0.0f,-(nearZ * farZ) / (farZ - nearZ),0.0f }
		};
		return Matrix4(tmp);
	}

	static const Matrix4 kIdentity;
};

inline Matrix4 operator*(const Matrix4& a, const Matrix4& b)
{
	Matrix4 result = {};
	result.m[0][0] = a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0] + a.m[0][2] * b.m[2][0] + a.m[0][3] * b.m[3][0];
	result.m[0][1] = a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1] + a.m[0][2] * b.m[2][1] + a.m[0][3] * b.m[3][1];
	result.m[0][2] = a.m[0][0] * b.m[0][2] + a.m[0][1] * b.m[1][2] + a.m[0][2] * b.m[2][2] + a.m[0][3] * b.m[3][2];
	result.m[0][3] = a.m[0][0] * b.m[0][3] + a.m[0][1] * b.m[1][3] + a.m[0][2] * b.m[2][3] + a.m[0][3] * b.m[3][3];
	result.m[1][0] = a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0] + a.m[1][2] * b.m[2][0] + a.m[1][3] * b.m[3][0];
	result.m[1][1] = a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1] + a.m[1][2] * b.m[2][1] + a.m[1][3] * b.m[3][1];
	result.m[1][2] = a.m[1][0] * b.m[0][2] + a.m[1][1] * b.m[1][2] + a.m[1][2] * b.m[2][2] + a.m[1][3] * b.m[3][2];
	result.m[1][3] = a.m[1][0] * b.m[0][3] + a.m[1][1] * b.m[1][3] + a.m[1][2] * b.m[2][3] + a.m[1][3] * b.m[3][3];
	result.m[2][0] = a.m[2][0] * b.m[0][0] + a.m[2][1] * b.m[1][0] + a.m[2][2] * b.m[2][0] + a.m[2][3] * b.m[3][0];
	result.m[2][1] = a.m[2][0] * b.m[0][1] + a.m[2][1] * b.m[1][1] + a.m[2][2] * b.m[2][1] + a.m[2][3] * b.m[3][1];
	result.m[2][2] = a.m[2][0] * b.m[0][2] + a.m[2][1] * b.m[1][2] + a.m[2][2] * b.m[2][2] + a.m[2][3] * b.m[3][2];
	result.m[2][3] = a.m[2][0] * b.m[0][3] + a.m[2][1] * b.m[1][3] + a.m[2][2] * b.m[2][3] + a.m[2][3] * b.m[3][3];
	result.m[3][0] = a.m[3][0] * b.m[0][0] + a.m[3][1] * b.m[1][0] + a.m[3][2] * b.m[2][0] + a.m[3][3] * b.m[3][0];
	result.m[3][1] = a.m[3][0] * b.m[0][1] + a.m[3][1] * b.m[1][1] + a.m[3][2] * b.m[2][1] + a.m[3][3] * b.m[3][1];
	result.m[3][2] = a.m[3][0] * b.m[0][2] + a.m[3][1] * b.m[1][2] + a.m[3][2] * b.m[2][2] + a.m[3][3] * b.m[3][2];
	result.m[3][3] = a.m[3][0] * b.m[0][3] + a.m[3][1] * b.m[1][3] + a.m[3][2] * b.m[2][3] + a.m[3][3] * b.m[3][3];
	return result;
}

inline Vector3 operator*(const Vector3& a, const Matrix4& b)
{
	return Vector3(
		a.x * b.m[0][0] + a.y * b.m[1][0] + a.z * b.m[2][0] + b.m[3][0],
		a.x * b.m[0][1] + a.y * b.m[1][1] + a.z * b.m[2][1] + b.m[3][1],
		a.x * b.m[0][2] + a.y * b.m[1][2] + a.z * b.m[2][2] + b.m[3][2]);
}

inline Matrix4& operator*=(Matrix4& a, const Matrix4& b)
{
	a = a * b;
	return a;
}

inline Vector3& operator*=(Vector3& a, const Matrix4& b)
{
	a = a * b;
	return a;
}


// 行列式
inline float Determinant(const Matrix4& a)
{
	return
		a.m[0][0] * ((a.m[1][1] * (a.m[2][2] * a.m[3][3] - a.m[2][3] * a.m[3][2])) + (a.m[1][2] * (a.m[2][3] * a.m[3][1] - a.m[2][1] * a.m[3][3])) + (a.m[1][3] * (a.m[2][1] * a.m[3][2] - a.m[2][2] * a.m[3][1]))) -
		a.m[0][1] * ((a.m[1][0] * (a.m[2][2] * a.m[3][3] - a.m[2][3] * a.m[3][2])) + (a.m[1][2] * (a.m[2][3] * a.m[3][0] - a.m[2][0] * a.m[3][3])) + (a.m[1][3] * (a.m[2][0] * a.m[3][2] - a.m[2][2] * a.m[3][0]))) +
		a.m[0][2] * ((a.m[1][0] * (a.m[2][1] * a.m[3][3] - a.m[2][3] * a.m[3][1])) + (a.m[1][1] * (a.m[2][3] * a.m[3][0] - a.m[2][0] * a.m[3][3])) + (a.m[1][3] * (a.m[2][0] * a.m[3][1] - a.m[2][1] * a.m[3][0]))) -
		a.m[0][3] * ((a.m[1][0] * (a.m[2][1] * a.m[3][2] - a.m[2][2] * a.m[3][1])) + (a.m[1][1] * (a.m[2][2] * a.m[3][0] - a.m[2][0] * a.m[3][2])) + (a.m[1][2] * (a.m[2][0] * a.m[3][1] - a.m[2][1] * a.m[3][0])));
}

// 逆行列
inline Matrix4 Inverse(const Matrix4& a)
{
	float det =
		a.m[0][0] * ((a.m[1][1] * (a.m[2][2] * a.m[3][3] - a.m[2][3] * a.m[3][2])) + (a.m[1][2] * (a.m[2][3] * a.m[3][1] - a.m[2][1] * a.m[3][3])) + (a.m[1][3] * (a.m[2][1] * a.m[3][2] - a.m[2][2] * a.m[3][1]))) -
		a.m[0][1] * ((a.m[1][0] * (a.m[2][2] * a.m[3][3] - a.m[2][3] * a.m[3][2])) + (a.m[1][2] * (a.m[2][3] * a.m[3][0] - a.m[2][0] * a.m[3][3])) + (a.m[1][3] * (a.m[2][0] * a.m[3][2] - a.m[2][2] * a.m[3][0]))) +
		a.m[0][2] * ((a.m[1][0] * (a.m[2][1] * a.m[3][3] - a.m[2][3] * a.m[3][1])) + (a.m[1][1] * (a.m[2][3] * a.m[3][0] - a.m[2][0] * a.m[3][3])) + (a.m[1][3] * (a.m[2][0] * a.m[3][1] - a.m[2][1] * a.m[3][0]))) -
		a.m[0][3] * ((a.m[1][0] * (a.m[2][1] * a.m[3][2] - a.m[2][2] * a.m[3][1])) + (a.m[1][1] * (a.m[2][2] * a.m[3][0] - a.m[2][0] * a.m[3][2])) + (a.m[1][2] * (a.m[2][0] * a.m[3][1] - a.m[2][1] * a.m[3][0])));
	MY_ASSERT(std::abs(det) > MyMath::kEpsilon);
	float oneOverDet = 1.0f / det;
	Matrix4 result = {};
	result.m[0][0] = (a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1]) * oneOverDet;
	result.m[0][1] = (a.m[0][2] * a.m[2][1] - a.m[0][1] * a.m[2][2]) * oneOverDet;
	result.m[0][2] = (a.m[0][1] * a.m[1][2] - a.m[0][2] * a.m[1][1]) * oneOverDet;
	result.m[0][3] = 0.0f;
	result.m[1][0] = (a.m[1][2] * a.m[2][0] - a.m[1][0] * a.m[2][2]) * oneOverDet;
	result.m[1][1] = (a.m[0][0] * a.m[2][2] - a.m[0][2] * a.m[2][0]) * oneOverDet;
	result.m[1][2] = (a.m[0][2] * a.m[1][0] - a.m[0][0] * a.m[1][2]) * oneOverDet;
	result.m[1][3] = 0.0f;
	result.m[2][0] = (a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0]) * oneOverDet;
	result.m[2][1] = (a.m[0][1] * a.m[2][0] - a.m[0][0] * a.m[2][1]) * oneOverDet;
	result.m[2][2] = (a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0]) * oneOverDet;
	result.m[2][3] = 0.0f;
	result.m[3][0] = -(a.m[3][0] * result.m[0][0] + a.m[3][1] * result.m[1][0] + a.m[3][2] * result.m[2][0]);
	result.m[3][1] = -(a.m[3][0] * result.m[0][1] + a.m[3][1] * result.m[1][1] + a.m[3][2] * result.m[2][1]);
	result.m[3][2] = -(a.m[3][0] * result.m[0][2] + a.m[3][1] * result.m[1][2] + a.m[3][2] * result.m[2][2]);
	result.m[3][3] = 1.0f;
	return result;
}

// 転置
inline Matrix4 Transpose(const Matrix4& a)
{
	Matrix4 result = {};
	result.m[0][0] = a.m[0][0];
	result.m[0][1] = a.m[1][0];
	result.m[0][2] = a.m[2][0];
	result.m[0][3] = a.m[3][0];
	result.m[1][0] = a.m[0][1];
	result.m[1][1] = a.m[1][1];
	result.m[1][2] = a.m[2][1];
	result.m[1][3] = a.m[3][1];
	result.m[2][0] = a.m[0][2];
	result.m[2][1] = a.m[1][2];
	result.m[2][2] = a.m[2][2];
	result.m[2][3] = a.m[3][2];
	result.m[3][0] = a.m[0][3];
	result.m[3][1] = a.m[1][3];
	result.m[3][2] = a.m[2][3];
	result.m[3][3] = a.m[3][3];
	return result;
}
