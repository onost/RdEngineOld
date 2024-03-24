#pragma once
#include "Actor/Actor.h"

class DemoCamera : public Actor
{
public:
	DemoCamera(Scene* scene);
	void ActorInput(const InputSystem::State& input) override;
	void ActorUpdate(float deltaTime) override;
	void ActorUpdateForDev() override;
	Type GetType() const { return Type::DemoCamera; }

private:
	const float kRotMax = MyMath::ToRadians(80.0f);
	const float kRotMin = MyMath::ToRadians(-45.0f);
	Actor* mTarget;
	float mDistance;
	float mRot;
	float mRotVel;
	float mRotSpeed;

	float mLeeway;
};
