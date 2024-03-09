#pragma once
#include "Actor/Actor.h"

class CameraComponent;
class SpriteRenderer;

class StarPieceManager : public Actor
{
public:
	StarPieceManager(Scene* scene);
	void ActorUpdate(float deltaTime) override;
	Type GetType() const { return Type::StarPieceManager; }

	void GetStarPiece();

private:
	static const uint32_t kStarPieceMax = 5;
	uint32_t mStarPieceCount;
	SpriteRenderer* mStarPieceSprites[kStarPieceMax];
	CameraComponent* mPlayerCam;
	CameraComponent* mBreakCam;
	float mTimer;
};
