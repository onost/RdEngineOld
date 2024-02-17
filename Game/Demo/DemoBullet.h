#pragma once
#include "Actor/Actor.h"

// 弾
class DemoBullet : public Actor
{
public:
	DemoBullet(Scene* scene);
	void ActorUpdate(float deltaTime) override;
	void OnCollision(Actor* other, CollisionInfo* /*info*/) override;

private:
	float mLife;
	float mSpeed;
};
