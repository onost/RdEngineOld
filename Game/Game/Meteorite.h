#pragma once
#include "Actor/Actor.h"

class GravityBody;

// 隕石
class Meteorite : public Actor
{
public:
	Meteorite(Scene* scene);
	void ActorUpdate(float deltaTime) override;
	void ActorOnCollisionEnter(Actor*, CollisionInfo*) override;
	Type GetType() const { return Type::Meteorite; }
	void SetVelocity(const Vector3& velocity) { mVelocity = velocity; }

	void SetShadowIndex(uint32_t shadowIndex) { mShadowIndex = shadowIndex; }
private:
	//GravityBody* mGravityBody;
	Vector3 mVelocity;
	uint32_t mShadowIndex;
	Vector3 mRandRot;
};
