#include "DemoCamera.h"
#include "Collision/Collision.h"
#include "Collision/CollisionManager.h"
#include "Component/CameraComponent.h"
#include "Editor.h"
#include "Helper/Helper.h"
#include "Scene/Scene.h"

DemoCamera::DemoCamera(Scene* scene)
	: Actor(scene)
	, mTarget(nullptr)
	, mDistance(10.0f)
	, mRot(MyMath::kPiOver2 * 0.2f)
	, mRotVel(0.0f)
	, mRotSpeed(MyMath::kPiOver2)
	, mLeeway(1.5f)
{
	new CameraComponent(this);
}

void DemoCamera::ActorInput(const InputSystem::State& input)
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

void DemoCamera::ActorUpdate(float deltaTime)
{
	if (!mTarget)
	{
		mTarget = mScene->GetActor("Demo Player");
	}
	if (mTarget)
	{
		mRot += mRotVel * mRotSpeed * deltaTime;
		mRot = MyMath::Clamp(mRot, kRotMin, kRotMax);

		Quaternion rotation = mTarget->mTransform->mRotation;
		Vector3 b = Vector3(0.0f, 0.0f, -1.0f) * rotation;
		Vector3 r = Vector3(1.0f, 0.0f, 0.0f) * rotation;
		Quaternion rot = Quaternion(r, mRot);

		const Vector3 corrPos = Vector3(0.0f, 2.0f, 0.0f);// 少し上
		mTransform->mPosition = mTarget->mTransform->mPosition + corrPos + b * mDistance * rot;
		mTransform->mRotation = rotation * rot;

		// カメラが埋まらないように
		Ray ray = Ray(mTarget->mTransform->mPosition + corrPos, mTransform->mPosition + b * mLeeway);// 少し長め
		RaycastInfo info = {};
		if (mScene->GetCollisionManager()->Raycast(ray, info, CollisionAttr::Terrain))
		{
			float dist = Length(info.mPoint - ray.mStart);
			if (dist <= mDistance + mLeeway)
			{
				mTransform->mPosition = info.mPoint + -b * mLeeway;
			}
		}
	}

	/*Actor* skydome = mScene->GetActor("Skydome");
	if (skydome)
	{
		skydome->mTransform->mPosition = mTransform->mPosition;
	}*/
}

// ==================================================
// 開発用
// ==================================================

void DemoCamera::ActorUpdateForDev()
{
	ImGui::Text("This is camera!");
	ImGui::DragFloat("Leeway", &mLeeway, 0.01f, 0.0f, mDistance);
}
