#pragma once
#include "Graphics/Camera.h"
#include "Input/InputSystem.h"
#include <memory>

class DebugCamera
{
public:
	DebugCamera();
	void Input(const InputSystem::State& input);
	void Update(float deltaTime);
	Camera* GetCamera() const { return mCamera.get(); }

private:
	const float kRotSpeed = 2.0f;
	const float kSpeed = 0.05f;
	const float kZoomSpeed = 0.002f;
	const float kMinDist = 1.0f;
	const float kMaxDist = 500.0f;
	std::unique_ptr<Camera> mCamera;
	Vector3 mRotVel;
	Vector3 mVelocity;
	Vector3 mTarget;
	float mDistance;
};
