#pragma once

class Vector3;
class Matrix4;

// クォータニオン
class Quaternion
{
public:
	float w;
	float x;
	float y;
	float z;

	Quaternion();
	Quaternion(float w, float x, float y, float z);
	Quaternion(const Vector3& axis, float theta);

	void Normalize();

	static const Quaternion kIdentity;
};

Quaternion Conjugate(const Quaternion& q);
Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
Vector3 operator*(const Vector3& v, const Quaternion& q);
Quaternion operator*=(Quaternion& q1, const Quaternion& q2);
Vector3& operator*=(Vector3& v, const Quaternion& q);
float Dot(const Quaternion& q1, const Quaternion& q2);
Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t);
Quaternion Pow(const Quaternion& q, float e);
Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t);
float GetTheta(const Quaternion& q);
Vector3 GetAxis(const Quaternion& q);

Vector3 ToEuler(const Quaternion& q);
Vector3 ToEuler(const Matrix4& m);
Quaternion ToQuaternion(const Vector3& e);
Quaternion ToQuaternion(const Matrix4& m);
