#pragma once

class Vector3;
class Quaternion;

// 4x4行列
class Matrix4
{
public:
	float m[4][4];

	Matrix4();
	Matrix4(float m[4][4]);
	Vector3 GetScale() const;
	Quaternion GetRotation() const;
	Quaternion GetRotation(const Vector3& scale) const;
	Vector3 GetTranslation() const;

	static Matrix4 CreateScale(const Vector3& scale);
	static Matrix4 CreateScale(float x, float y, float z);
	static Matrix4 CreateScale(float scale);
	static Matrix4 CreateRotateX(float rotate);
	static Matrix4 CreateRotateY(float rotate);
	static Matrix4 CreateRotateZ(float rotate);
	static Matrix4 CreateRotate(const Quaternion& q);
	static Matrix4 CreateTranslate(const Vector3& trans);
	static Matrix4 CreateTranslate(float x, float y, float z);
	static Matrix4 CreateAffine(
		const Vector3& scale, const Quaternion& rotate, const Vector3& trans);

	static Matrix4 CreateLookAt(
		const Vector3& eye, const Vector3& target, const Vector3& up);
	static Matrix4 CreateOrthographic(
		float left, float top, float right, float bottom, float nearZ, float farZ);
	static Matrix4 CreatePerspectiveFov(
		float fovY, float aspectRatio, float nearZ, float farZ);

	static const Matrix4 kIdentity;
};

Matrix4 operator*(const Matrix4& m1, const Matrix4& m2);
Vector3 operator*(const Vector3& v, const Matrix4& m);
Matrix4 operator*=(Matrix4& m1, const Matrix4& m2);
Vector3& operator*=(Vector3& v, const Matrix4& m);
float Determinant(const Matrix4& m);
Matrix4 Inverse(const Matrix4& m);
Matrix4 Transpose(const Matrix4& m);
