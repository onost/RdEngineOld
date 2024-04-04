#include "MyMath.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "Vector3.h"

Vector3 ToEuler(const Matrix4& a)
{
	Vector3 euler = {};
	euler.x = std::atan2(a.m[2][1], a.m[2][2]);
	float sinY = -a.m[2][0];
	float cosY = std::sqrt(a.m[0][0] * a.m[0][0] + a.m[1][0] * a.m[1][0]);
	euler.y = std::atan2(sinY, cosY);
	float sinX = std::sin(euler.x);
	float cosX = std::cos(euler.x);
	euler.z = std::atan2(a.m[1][0] * cosX - a.m[0][0] * sinX, a.m[0][1] * cosX - a.m[1][1] * sinX);
	return euler;
}

Vector3 ToEuler(const Quaternion& a)
{
	Vector3 euler = {};
	float sinXcosY = 2.0f * (a.w * a.x + a.y * a.z);
	float cosXcosY = 1.0f - 2.0f * (a.x * a.x + a.y * a.y);
	euler.x = std::atan2(sinXcosY, cosXcosY);
	float sinY = 2.0f * (a.w * a.y - a.z * a.x);
	if (std::abs(sinY) >= 1.0f)
	{
		euler.y = MyMath::kPiOver2;
		if (sinY < 0.0f)
		{
			euler.y = -euler.y;
		}
	}
	else
	{
		euler.y = std::asin(sinY);
	}
	float sinZcosY = 2.0f * (a.w * a.z + a.x * a.y);
	float cosZcosY = 1.0f - 2.0f * (a.y * a.y + a.z * a.z);
	euler.z = std::atan2(sinZcosY, cosZcosY);
	return euler;
}

//Matrix4 ToMatrix(const Vector3& a);
//Matrix4 ToMatrix(const Quaternion& a);

Quaternion ToQuaternion(const Vector3& a)
{
	float cosX = std::cos(a.x * 0.5f);
	float sinX = std::sin(a.x * 0.5f);
	float cosY = std::cos(a.y * 0.5f);
	float sinY = std::sin(a.y * 0.5f);
	float cosZ = std::cos(a.z * 0.5f);
	float sinZ = std::sin(a.z * 0.5f);
	Quaternion quat = {};
	quat.w = cosZ * cosY * cosX + sinZ * sinY * sinX;
	quat.x = cosZ * cosY * sinX - sinZ * sinY * cosX;
	quat.y = sinZ * cosY * sinX + cosZ * sinY * cosX;
	quat.z = sinZ * cosY * cosX - cosZ * sinY * sinX;
	quat.Normalize();
	return quat;
}

Quaternion ToQuaternion(const Matrix4& a)
{
	auto t = Transpose(a);
	Quaternion quat = {};
	float trace = t.m[0][0] + t.m[1][1] + t.m[2][2];
	if (trace > 0.0f)
	{
		float s = 0.5f / std::sqrt(trace + 1.0f);
		quat.w = 0.25f / s;
		quat.x = (t.m[2][1] - t.m[1][2]) * s;
		quat.y = (t.m[0][2] - t.m[2][0]) * s;
		quat.z = (t.m[1][0] - t.m[0][1]) * s;
	}
	else
	{
		if (t.m[0][0] > t.m[1][1] && t.m[0][0] > t.m[2][2])
		{
			float s = 2.0f * std::sqrt(1.0f + t.m[0][0] - t.m[1][1] - t.m[2][2]);
			quat.w = (t.m[2][1] - t.m[1][2]) / s;
			quat.x = 0.25f * s;
			quat.y = (t.m[0][1] + t.m[1][0]) / s;
			quat.z = (t.m[0][2] + t.m[2][0]) / s;
		}
		else if (t.m[1][1] > t.m[2][2])
		{
			float s = 2.0f * std::sqrt(1.0f + t.m[1][1] - t.m[0][0] - t.m[2][2]);
			quat.w = (t.m[0][2] - t.m[2][0]) / s;
			quat.x = (t.m[0][1] + t.m[1][0]) / s;
			quat.y = 0.25f * s;
			quat.z = (t.m[1][2] + t.m[2][1]) / s;
		}
		else
		{
			float s = 2.0f * std::sqrt(1.0f + t.m[2][2] - t.m[0][0] - t.m[1][1]);
			quat.w = (t.m[1][0] - t.m[0][1]) / s;
			quat.x = (t.m[0][2] + t.m[2][0]) / s;
			quat.y = (t.m[1][2] + t.m[2][1]) / s;
			quat.z = 0.25f * s;
		}
	}
	return quat;
}
