#pragma once
#include "Actor/Actor.h"

class Animator;
class GravityBody;

// スターピース
class StarPiece : public Actor
{
public:
	StarPiece(Scene* scene);
	void ActorUpdate(float deltaTime) override;
	void ActorOnTriggerEnter(Actor* other) override;
	Type GetType() const { return Type::StarPiece; }

private:
	// アニメータ
	Animator* mAnimator;
	// 重力
	GravityBody* mGravityBody;
};
