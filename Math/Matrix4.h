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
	Quaternion GetRotate() const;
	Vector3 GetTranslate() const;

	// Scale
	static Matrix4 CreateScale(const Vector3& scale);
	static Matrix4 CreateScale(float scaleX, float scaleY, float scaleZ);
	static Matrix4 CreateScale(float scale);
	// Rotate
	static Matrix4 CreateRotateX(float rotateX);
	static Matrix4 CreateRotateY(float rotateY);
	static Matrix4 CreateRotateZ(float rotateZ);
	static Matrix4 CreateRotate(const Quaternion& quat);
	// Translate
	static Matrix4 CreateTranslate(const Vector3& trans);
	static Matrix4 CreateTranslate(float transX, float transY, float transZ);
	// Affine
	static Matrix4 CreateAffine(
		const Vector3& scale, const Quaternion& quat, const Vector3& trans);
	// Look at
	static Matrix4 CreateLookAt(
		const Vector3& eye, const Vector3& target, const Vector3& up);
	// Orthographic
	static Matrix4 CreateOrthographic(
		float left, float top, float right, float bottom, float nearZ, float farZ);
	// Perspective fov
	static Matrix4 CreatePerspectiveFov(
		float fovY, float aspectRatio, float nearZ, float farZ);

	static const Matrix4 kIdentity;
};

Matrix4 operator*(const Matrix4& a, const Matrix4& b);
Vector3 operator*(const Vector3& a, const Matrix4& b);
Matrix4& operator*=(Matrix4& a, const Matrix4& b);
Vector3& operator*=(Vector3& a, const Matrix4& b);
float Determinant(const Matrix4& a);// 行列式
Matrix4 Inverse(const Matrix4& a);// 逆行列
Matrix4 Transpose(const Matrix4& a);// 転置
