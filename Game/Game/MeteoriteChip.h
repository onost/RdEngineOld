#pragma once
#include "Actor/Actor.h"

class MeteoriteChip : public Actor
{
public:
	MeteoriteChip(Scene* scene);
	void ActorUpdate(float deltaTime) override;
	void SetVelocity(const Vector3& velocity) { mVelocity = velocity; }
	void SetDownVec(const Vector3& downVec) { mDownVec = downVec; }

private:
	Vector3 mVelocity;
	Vector3 mDownVec;
	Vector3 mRandRot;
};

