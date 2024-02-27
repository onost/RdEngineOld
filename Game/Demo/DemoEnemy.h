#pragma once
#include "Actor/Actor.h"

class DemoEnemy : public Actor
{
public:
	DemoEnemy(Scene* scene);
	void ActorUpdate(float deltaTime) override;
	void ActorOnCollision(Actor* other, CollisionInfo* /*info*/) override;
	void ActorOnTrigger(Actor* other) override;
	Type GetType() const { return Type::DemoEnemy; }

private:
	int mHp;
	float mRadius;
	Vector3 mVelocity;
	float mSpeed;
};
