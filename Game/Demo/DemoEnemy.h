#pragma once
#include "Actor/Actor.h"

class DemoEnemy : public Actor
{
public:
	DemoEnemy(Scene* scene);
	void ActorUpdate(float deltaTime) override;
	void ActorOnCollisionStay(Actor* other, CollisionInfo* /*info*/) override;
	void ActorOnTriggerEnter(Actor* other) override;
	Type GetType() const { return Type::DemoEnemy; }

private:
	int mHp;
	float mRadius;
	Vector3 mVelocity;
	float mSpeed;
};
