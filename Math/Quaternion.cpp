#include "Quaternion.h"
#include "Helper/MyAssert.h"
#include "Matrix4.h"
#include "MyMath.h"
#include "Vector3.h"

const Quaternion Quaternion::kIdentity(1.0f, 0.0f, 0.0f, 0.0f);

Quaternion::Quaternion()
{
	*this = kIdentity;
}

Quaternion::Quaternion(float w, float x, float y, float z)
	: w(w)
	, x(x)
	, y(y)
	, z(z)
{}

Quaternion::Quaternion(const Vector3& axis, float theta)
{
	MY_ASSERT(fabs(Length(axis) - 1.0f) <= 0.001f);//MyMath::kEpsilon);
	float thetaOver2 = theta * 0.5f;
	float sin = sinf(thetaOver2);
	w = cosf(thetaOver2);
	x = axis.x * sin;
	y = axis.y * sin;
	z = axis.z * sin;
}

void Quaternion::Normalize()
{
	float a = w * w + x * x + y * y + z * z;
	if (a > MyMath::kEpsilon)
	{
		float oneOverA = 1.0f / sqrt(a);
		w *= oneOverA;
		x *= oneOverA;
		y *= oneOverA;
		z *= oneOverA;
	}
	else
	{
		//MyAssert(false);
		*this = kIdentity;
	}
}

Quaternion Conjugate(const Quaternion& q)
{
	Quaternion result;
	result.w = q.w;
	result.x = -q.x;
	result.y = -q.y;
	result.z = -q.z;
	return result;
}

Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion result;
	result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
	result.x = q1.w * q2.x + q1.x * q2.w + q1.z * q2.y - q1.y * q2.z;
	result.y = q1.w * q2.y + q1.y * q2.w + q1.x * q2.z - q1.z * q2.x;
	result.z = q1.w * q2.z + q1.z * q2.w + q1.y * q2.x - q1.x * q2.y;
	return result;
}

Vector3 operator*(const Vector3& v, const Quaternion& q)
{
	Quaternion result;
	result = Conjugate(q) * Quaternion(0.0f, v.x, v.y, v.z) * q;
	return Vector3(result.x, result.y, result.z);
}

Quaternion operator*=(Quaternion& q1, const Quaternion& q2)
{
	q1 = q1 * q2;
	return q1;
}

Vector3& operator*=(Vector3& v, const Quaternion& q)
{
	v = v * q;
	return v;
}

float Dot(const Quaternion& q1, const Quaternion& q2)
{
	return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
}

Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	Quaternion result;
	result.w = MyMath::Lerp(q1.w, q2.w, t);
	result.x = MyMath::Lerp(q1.x, q2.x, t);
	result.y = MyMath::Lerp(q1.y, q2.y, t);
	result.z = MyMath::Lerp(q1.z, q2.z, t);
	result.Normalize();
	return result;
}

Quaternion Pow(const Quaternion& q, float e)
{
	if (fabs(q.w) > 0.999f)
	{
		return q;
	}
	float alpha = acosf(q.w);
	float newAlpha = alpha * e;
	Quaternion result;
	result.w = cosf(newAlpha);
	float a = sinf(newAlpha) / sinf(alpha);
	result.x = q.x * a;
	result.y = q.y * a;
	result.z = q.z * a;
	return result;
}

Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	if (t <= 0.0f)
	{
		return q1;
	}
	if (t >= 1.0f)
	{
		return q2;
	}
	float cosOmega = Dot(q1, q2);
	float w = q2.w;
	float x = q2.x;
	float y = q2.y;
	float z = q2.z;
	if (cosOmega < 0.0f)
	{
		w = -q2.w;
		x = -q2.x;
		y = -q2.y;
		z = -q2.z;
		cosOmega = -cosOmega;
	}
	float k0, k1;
	if (cosOmega > 0.999f)
	{
		k0 = 1.0f - t;
		k1 = t;
	}
	else
	{
		float sinOmega = sqrtf(1.0f - cosOmega * cosOmega);
		float omega = atan2f(sinOmega, cosOmega);
		float oneOverSin = 1.0f / sinOmega;
		k0 = sinf((1.0f - t) * omega) * oneOverSin;
		k1 = sinf(t * omega) * oneOverSin;
	}
	Quaternion result;
	result.w = k0 * q1.w + k1 * w;
	result.x = k0 * q1.x + k1 * x;
	result.y = k0 * q1.y + k1 * y;
	result.z = k0 * q1.z + k1 * z;
	return result;
}

float GetTheta(const Quaternion& q)
{
	return acosf(q.w) * 2.0f;
}

Vector3 GetAxis(const Quaternion& q)
{
	float sinSq = 1.0f - q.w * q.w;
	if (sinSq <= 0.0f)
	{
		return Vector3(1.0f, 0.0f, 0.0f);
	}
	float oneOverSin = 1.0f / sqrtf(sinSq);
	return Vector3(q.x * oneOverSin, q.y * oneOverSin, q.z * oneOverSin);
}

Vector3 ToEuler(const Quaternion& q)
{
	Vector3 euler;
	float sxcy = 2.0f * (q.w * q.x + q.y * q.z);
	float cxcy = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	euler.x = atan2f(sxcy, cxcy);
	float sy = 2.0f * (q.w * q.y - q.z * q.x);
	if (fabsf(sy) >= 1.0f)
	{
		euler.y = MyMath::kPiOver2;
		if (sy < 0.0f)
		{
			euler.y = -euler.y;
		}
	}
	else
	{
		euler.y = asinf(sy);
	}
	float szcy = 2.0f * (q.w * q.z + q.x * q.y);
	float czcy = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	euler.z = atan2f(szcy, czcy);
	return euler;
}

Vector3 ToEuler(const Matrix4& m)
{
	Vector3 euler;
	euler.x = atan2f(m.m[2][1], m.m[2][2]);
	float sy = -m.m[2][0];
	float cy = sqrtf(m.m[0][0] * m.m[0][0] + m.m[1][0] * m.m[1][0]);
	euler.y = atan2f(sy, cy);
	float sx = sinf(euler.x);
	float cx = cosf(euler.x);
	euler.z = atan2f(m.m[1][0] * cx - m.m[0][0] * sx, m.m[0][1] * cx - m.m[1][1] * sx);
	return euler;
}

Quaternion ToQuaternion(const Vector3& e)
{
	float cx = cosf(e.x * 0.5f);
	float sx = sinf(e.x * 0.5f);
	float cy = cosf(e.y * 0.5f);
	float sy = sinf(e.y * 0.5f);
	float cz = cosf(e.z * 0.5f);
	float sz = sinf(e.z * 0.5f);
	Quaternion result;
	result.w = cz * cy * cx + sz * sy * sx;
	result.x = cz * cy * sx - sz * sy * cx;
	result.y = sz * cy * sx + cz * sy * cx;
	result.z = sz * cy * cx - cz * sy * sx;
	result.Normalize();
	return result;
}

Quaternion ToQuaternion(const Matrix4& m)
{
	/*float w = m.m[0][0] + m.m[1][1] + m.m[2][2];
	float x = m.m[0][0] - m.m[1][1] - m.m[2][2];
	float y = -m.m[0][0] + m.m[1][1] - m.m[2][2];
	float z = -m.m[0][0] - m.m[1][1] + m.m[2][2];
	int i = 0;
	float max = w;
	if (x > max)
	{
		max = x;
		i = 1;
	}
	if (y > max)
	{
		max = y;
		i = 2;
	}
	if (z > max)
	{
		max = z;
		i = 3;
	}
	max = sqrtf(max + 1.0f) * 0.5f;
	float a = 0.25f / max;
	Quaternion result;
	switch (i)
	{
	case 0:
		result.w = max;
		result.x = (m.m[2][1] - m.m[1][2]) * a;
		result.y = (m.m[0][2] - m.m[2][0]) * a;
		result.z = (m.m[1][0] - m.m[0][1]) * a;
		break;
	case 1:
		result.x = max;
		result.w = (m.m[2][1] - m.m[1][2]) * a;
		result.y = (m.m[1][0] + m.m[0][1]) * a;
		result.z = (m.m[0][2] + m.m[2][0]) * a;
		break;
	case 2:
		result.y = max;
		result.w = (m.m[0][2] - m.m[2][0]) * a;
		result.x = (m.m[1][0] + m.m[0][1]) * a;
		result.z = (m.m[2][1] + m.m[1][2]) * a;
		break;
	case 3:
		result.z = max;
		result.w = (m.m[1][0] - m.m[0][1]) * a;
		result.x = (m.m[0][2] + m.m[2][0]) * a;
		result.y = (m.m[2][1] + m.m[1][2]) * a;
		break;
	}
	return result;*/

	/*float w = m.m[0][0] + m.m[1][1] + m.m[2][2];
	float x = m.m[0][0] - m.m[1][1] - m.m[2][2];
	float y = -m.m[0][0] + m.m[1][1] - m.m[2][2];
	float z = -m.m[0][0] - m.m[1][1] + m.m[2][2];
	int i = 0;
	float max = w;
	if (x > max)
	{
		max = x;
		i = 1;
	}
	if (y > max)
	{
		max = y;
		i = 2;
	}
	if (z > max)
	{
		max = z;
		i = 3;
	}
	max = sqrtf(max + 1.0f) * 0.5f;
	float a = 0.25f / max;
	Quaternion result;
	switch (i)
	{
	case 0:
		result.w = max;
		result.x = (m.m[1][2] - m.m[2][1]) * a;
		result.y = (m.m[2][0] - m.m[0][2]) * a;
		result.z = (m.m[0][1] - m.m[1][0]) * a;
		break;
	case 1:
		result.x = max;
		result.w = (m.m[1][2] - m.m[2][1]) * a;
		result.y = (m.m[1][0] + m.m[0][1]) * a;//
		result.z = (m.m[2][0] + m.m[0][2]) * a;
		break;
	case 2:
		result.y = max;
		result.w = (m.m[2][0] - m.m[0][2]) * a;
		result.x = (m.m[1][0] + m.m[0][1]) * a;//
		result.z = (m.m[2][1] + m.m[1][2]) * a;//
		break;
	case 3:
		result.z = max;
		result.w = (m.m[0][1] - m.m[1][0]) * a;
		result.x = (m.m[2][0] + m.m[0][2]) * a;
		result.y = (m.m[2][1] + m.m[1][2]) * a;//
		break;
	}
	return result;*/

	auto t = Transpose(m);
	auto a = t.m;
	Quaternion result;
	// https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
	float trace = a[0][0] + a[1][1] + a[2][2]; // + 1.0f を削除しました。イーサンとのディスカッションを参照
	if (trace > 0) {// M_EPSILON を 0 に変更しました
		float s = 0.5f / sqrtf(trace + 1.0f);
		result.w = 0.25f / s;
		result.x = (a[2][1] - a[1][2]) * s;
		result.y = (a[0][2] - a[2][0]) * s;
		result.z = (a[1][0] - a[0][1]) * s;
	}
	else {
		if (a[0][0] > a[1][1] && a[0][0] > a[2][2]) {
			float s = 2.0f * sqrtf(1.0f + a[0][0] - a[1][1] - a[2][2]);
			result.w = (a[2][1] - a[1][2]) / s;
			result.x = 0.25f * s;
			result.y = (a[0][1] + a[1][0]) / s;
			result.z = (a[0][2] + a[2][0]) / s;
		}
		else if (a[1][1] > a[2][2]) {
			float s = 2.0f * sqrtf(1.0f + a[1][1] - a[0][0] - a[2][2]);
			result.w = (a[0][2] - a[2][0]) / s;
			result.x = (a[0][1] + a[1][0]) / s;
			result.y = 0.25f * s;
			result.z = (a[1][2] + a[2][1]) / s;
		}
		else {
			float s = 2.0f * sqrtf(1.0f + a[2][2] - a[0][0] - a[1][1]);
			result.w = (a[1][0] - a[0][1]) / s;
			result.x = (a[0][2] + a[2][0]) / s;
			result.y = (a[1][2] + a[2][1]) / s;
			result.z = 0.25f * s;
		}
	}
	return result;
}
