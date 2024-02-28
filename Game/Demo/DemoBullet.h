#pragma once
#include "Actor/Actor.h"

// 弾
class DemoBullet : public Actor
{
public:
	DemoBullet(Scene* scene);
	void ActorUpdate(float deltaTime) override;
	void ActorOnTriggerEnter(Actor* other) override;

private:
	float mLife;
	float mSpeed;
};
