#include "PlayerCamera.h"
#include "Collision/Collision.h"
#include "Collision/CollisionManager.h"
#include "Component/CameraComponent.h"
#include "Editor.h"
#include "Helper/Helper.h"
#include "Scene/Scene.h"

PlayerCamera::PlayerCamera(Scene* scene)
	: Actor(scene)
	, mTarget(nullptr)
	, mDistance(22.0f)
	, mRot(MyMath::kPiOver2 * 0.2f)
	, mRotVel(0.0f)
	, mRotSpeed(MyMath::kPiOver2)
	, mLeeway(1.5f)
{
	new CameraComponent(this);
}

void PlayerCamera::ActorInput(const InputSystem::State& input)
{
	// スティック
	mRotVel = -input.mGamepad.GetRStick().y;
	// キーボード
	if (input.mKeyboard.GetKey(DIK_UP))
	{
		mRotVel += -1.0f;
	}
	if (input.mKeyboard.GetKey(DIK_DOWN))
	{
		mRotVel += 1.0f;
	}
}

void PlayerCamera::ActorUpdate(float deltaTime)
{
	if (!mTarget)
	{
		mTarget = mScene->GetActor("Player");
		if (mTarget)
		{
			// Rotation
			Quaternion rotation = mTarget->mTransform->mRotation;
			Vector3 b = Vector3(0.0f, 0.0f, -1.0f) * rotation;
			Vector3 r = Vector3(1.0f, 0.0f, 0.0f) * rotation;
			mRot += mRotVel * mRotSpeed * deltaTime;
			mRot = MyMath::Clamp(mRot, kRotMin, kRotMax);
			Quaternion rot = Quaternion(r, mRot);
			// Position
			Vector3 corrPos = Vector3(0.0f, 2.0f, 0.0f) * rotation;

			mTransform->mRotation = rotation * rot;
			mTransform->mPosition = mTarget->mTransform->mPosition + corrPos + b * mDistance * rot;
		}
	}
	if (mTarget)
	{
		// Rotation
		Quaternion rotation = mTarget->mTransform->mRotation;
		Vector3 b = Vector3(0.0f, 0.0f, -1.0f) * rotation;
		Vector3 r = Vector3(1.0f, 0.0f, 0.0f) * rotation;
		mRot += mRotVel * mRotSpeed * deltaTime;
		mRot = MyMath::Clamp(mRot, kRotMin, kRotMax);
		Quaternion rot = Quaternion(r, mRot);
		// Position
		Vector3 corrPos = Vector3(0.0f, 2.0f, 0.0f) * rotation;
		Vector3 position = mTarget->mTransform->mPosition + corrPos + b * mDistance * rot;

		const float kSpeed = 0.2f;
		mTransform->mRotation = Slerp(mTransform->mRotation, rotation * rot, kSpeed);
		mTransform->mPosition = MyMath::Lerp(mTransform->mPosition, position, kSpeed);

		// カメラが埋まらないように
		Ray ray = Ray(mTarget->mTransform->mPosition + corrPos, mTransform->mPosition + b * mLeeway);// 少し長め
		RaycastInfo info = {};
		CollisionAttr attr = CollisionAttr(CollisionAttr::All & ~CollisionAttr::Allies);
		if (mScene->GetCollisionManager()->Raycast(ray, info, attr))
		{
			float dist = Length(info.mPoint - ray.mStart);
			if (dist <= mDistance + mLeeway)
			{
				mTransform->mPosition = info.mPoint + -b * mLeeway;
			}
		}
	}

	Actor* skydome = mScene->GetActor("Skydome");
	if (skydome)
	{
		skydome->mTransform->mPosition = mTransform->mPosition;
	}
}

// ==================================================
// 開発用
// ==================================================

void PlayerCamera::ActorUpdateForDev()
{
	ImGui::Text("This is camera!");
	ImGui::DragFloat("Leeway", &mLeeway, 0.01f, 0.0f, mDistance);
}
