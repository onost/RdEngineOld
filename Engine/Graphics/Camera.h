#pragma once
#include "Matrix4.h"
#include "Quaternion.h"
#include "Vector3.h"

// カメラ
class Camera
{
public:
	Camera();
	void UpdateViewProj();

	// アクセッサ
	const Quaternion& GetRotation() const { return mRotation; }
	const Vector3& GetPosition() const { return mPosition; }
	float GetFov() const { return mFov; }
	float GetNearZ() const { return mNearZ; }
	float GetFarZ() const { return mFarZ; }
	const Matrix4& GetView() const { return mView; }
	const Matrix4& GetProj() const { return mProj; }
	const Matrix4& GetViewProj() const { return mViewProj; }
	const Matrix4& GetBillboard() const { return mBillboard; }
	void SetRotation(const Quaternion& rotation) { mRotation = rotation; }
	void SetPosition(const Vector3& position) { mPosition = position; }
	void SetFov(float fov) { mFov = fov; }
	void SetNearZ(float nearZ) { mNearZ = nearZ; }
	void SetFarZ(float farZ) { mFarZ = farZ; }
	void SetView(const Matrix4& view) { mView = view; }

private:
	// View
	Quaternion mRotation;
	Vector3 mPosition;
	// Projection
	float mFov;
	float mNearZ;
	float mFarZ;

	Matrix4 mView;
	Matrix4 mProj;
	Matrix4 mViewProj;
	Matrix4 mBillboard;
	//Matrix4 mAxisBillboard[3];
};
