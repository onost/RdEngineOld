#pragma once

class Vector2;

// 3x3行列
class Matrix3
{
public:
	float m[3][3];

	Matrix3();
	Matrix3(float m[3][3]);
	Vector2 GetTranslation();

	static Matrix3 CreateScale(const Vector2& scale);
	static Matrix3 CreateScale(float x, float y);
	static Matrix3 CreateScale(float scale);
	static Matrix3 CreateRotate(float rotate);
	static Matrix3 CreateTranslate(const Vector2& trans);
	static Matrix3 CreateTranslate(float x, float y);
	static Matrix3 CreateAffine(
		const Vector2& scale, float rotate, const Vector2& trans);

	static const Matrix3 kIdentity;
};

Matrix3 operator*(const Matrix3& m1, const Matrix3& m2);
Vector2 operator*(const Vector2& v, const Matrix3& m);
Matrix3 operator*=(Matrix3& m1, const Matrix3& m2);
Vector2& operator*=(Vector2& v, const Matrix3& m);
float Determinant(const Matrix3& m);
Matrix3 Inverse(const Matrix3& m);
Matrix3 Transpose(const Matrix3& m);
