#pragma once
#include "Actor/Actor.h"

// 隕石発生装置
class MeteoriteEmitter : public Actor
{
public:
	MeteoriteEmitter(Scene* scene);
	void ActorUpdate(float deltaTime) override;
	void ActorUpdateForDev() override;
	void ActorRenderForDev(Primitive* prim);
	Type GetType() const { return Type::MeteoriteEmitter; }
private:
	// 隕石を発生させる
	void Emit();

private:
	float mFrequency;// 頻度
	float mInitFreq;
	float mRadius;// 半径
	uint32_t mShadowIndex;
};
