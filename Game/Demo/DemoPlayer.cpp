#include "DemoPlayer.h"
#include "Collision/CollisionManager.h"
#include "Component/MeshRenderer.h"
#include "Component/ParticleRenderer.h"
#include "Component/SphereCollider.h"
#include "Editor.h"
#include "Graphics/Renderer.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"

#include "DemoBullet.h"
#include "DemoCamera.h"

#include "RdEngine.h"
#include "Audio/AudioSystem.h"

DemoPlayer::DemoPlayer(Scene* scene)
	: Actor(scene)
	, mRadius(1.0f)
	, mVelocity(Vector3::kZero)
	, mSpeed(10.0f)
	, mRotVel(0.0f)
	, mRotSpeed(MyMath::kPiOver2)
	, mIsGround(false)
	, mJumpPower(0.0f)
	, mGravity(0.0f)
	, mGroundDist(0.3f)
	, mMaxGround(30.0f)
	, mInitCooldown(0.1f)
	, mCooldown(0.1f)
	, mSandSmoke(nullptr)
{
	// メッシュ
	auto mr = new MeshRenderer(this);
	mr->SetModel(mScene->GetRenderer()->GetModel("Player.obj"));
	// コライダー
	auto sc = new SphereCollider(this);
	sc->SetAttribute(CollisionAttr::Allies);// 味方
	sc->SetSphere({ {0.0f,0.0f,0.0f},mRadius });

	// 砂煙
	Actor* sandSmoker = new Actor(mScene);
	sandSmoker->SetName("Sand Smoker");
	sandSmoker->SetParent(this);
	mSandSmoke = new ParticleRenderer(sandSmoker);// パーティクル
}

void DemoPlayer::ActorInput(const InputSystem::State& input)
{
	// ==================================================
	// 移動
	// ==================================================

	// スティック
	Vector2 move = input.mGamepad.GetLStick();
	// キーボード
	if (input.mKeyboard.GetKey(DIK_W))
	{
		move.y += 1.0f;
	}
	if (input.mKeyboard.GetKey(DIK_S))
	{
		move.y += -1.0f;
	}
	if (input.mKeyboard.GetKey(DIK_A))
	{
		move.x += -1.0f;
	}
	if (input.mKeyboard.GetKey(DIK_D))
	{
		move.x += 1.0f;
	}
	move.Normalize();
	mVelocity.x = move.x * mSpeed;
	mVelocity.z = move.y * mSpeed;

	// ==================================================
	// 回転
	// ==================================================

	// スティック
	mRotVel = input.mGamepad.GetRStick().x;
	// キーボード
	if (input.mKeyboard.GetKey(DIK_LEFT))
	{
		mRotVel += -1.0f;
	}
	if (input.mKeyboard.GetKey(DIK_RIGHT))
	{
		mRotVel += 1.0f;
	}

	mRotVel = MyMath::Clamp(mRotVel, -1.0f, 1.0f);

	// ==================================================
	// ジャンプ
	// ==================================================
	if (mIsGround)
	{
		if (input.mGamepad.GetButtonDown(XINPUT_GAMEPAD_A) ||
			input.mKeyboard.GetKeyDown(DIK_SPACE))
		{
			mVelocity.y = mJumpPower;
			//auto audio = gEngine->GetAudio();
			//audio->Play(audio->Load("Assets/Audio/Audio01.wav"));
		}
	}

	// ==================================================
	// 弾
	// ==================================================
	if (input.mGamepad.GetRTrigger() > 0.0f ||
		input.mKeyboard.GetKey(DIK_LSHIFT))
	{
		if (mCooldown <= 0.0f)
		{
			DemoBullet* bullet = new DemoBullet(mScene);
			bullet->mTransform->mPosition = mTransform->mPosition;
			bullet->mTransform->mRotation = mTransform->mRotation;
			mCooldown = mInitCooldown;
		}
	}
}

void DemoPlayer::ActorUpdate(float deltaTime)
{
	mCooldown = MyMath::Max(0.0f, mCooldown - deltaTime);

	// ==================================================
	// 回転
	// ==================================================
	Quaternion rot = Quaternion(Vector3(0.0f, 1.0f, 0.0f), mRotVel * mRotSpeed * deltaTime);
	mTransform->mRotation *= rot;

	// ==================================================
	// 移動
	// ==================================================
	Vector3 r = Vector3(1.0f, 0.0f, 0.0f) * mTransform->mRotation;// right
	Vector3 f = Vector3(0.0f, 0.0f, 1.0f) * mTransform->mRotation;// forward
	mTransform->mPosition += r * mVelocity.x * deltaTime;
	mTransform->mPosition += f * mVelocity.z * deltaTime;

	// 重力
	// 地面との判定より前
	if (!mIsGround)
	{
		mVelocity.y += -mGravity;
	}
	mTransform->mPosition.y += mVelocity.y * deltaTime;

	// 地面
	mIsGround = false;
	Ray ray = Ray(mTransform->mPosition, mTransform->mPosition + Vector3(0.0f, -1.0f, 0.0f));// 下へ
	if (mVelocity.y < 0.0f)
	{
		// 落下中は少し上から
		ray.mStart.y += -mVelocity.y * deltaTime;
	}
	RaycastInfo info = {};
	CollisionAttr attr = CollisionAttr(CollisionAttr::All & ~CollisionAttr::Allies);// 味方以外
	if (mScene->GetCollisionManager()->Raycast(ray, info, attr))
	{
		float dist = Length(info.mPoint - ray.mStart);
		if (dist <= mRadius + mGroundDist)
		{
			/*Helper::WriteToConsole(std::format("({:6.3f},{:6.3f},{:6.3f})\n",
				info.mNormal.x,
				info.mNormal.y,
				info.mNormal.z));*/

			/*float maxCos = cosf(MyMath::ToRadians(mMaxGround));
			Helper::WriteToConsole(std::format("{}\n", MyMath::ToDegrees(acosf(info.mNormal.y))));
			if (info.mNormal.y >= maxCos)
			{
				mIsGround = true;
				mVelocity.y = 0.0f;
			}
			if (mVelocity.y >= 0.0f)
			{
				// 押し戻し
				mTransform->mPosition = info.mPoint + Vector3(0.0f, mRadius, 0.0f);
			}*/

			mIsGround = true;
			mVelocity.y = 0.0f;
			// 押し戻し
			mTransform->mPosition = info.mPoint + Vector3(0.0f, mRadius, 0.0f);

			// 奈落
			if (info.mActor->GetName() == "Abyss")
			{
				mTransform->mPosition = Vector3(0.0f, 2.0f, -10.0f);
				mTransform->mRotation = Quaternion::kIdentity;
				mVelocity = Vector3::kZero;
			}
		}
	}

	// 砂煙
	if (Length(mVelocity) && mIsGround)
	{
		mSandSmoke->Play();
	}
	else
	{
		mSandSmoke->Stop();
	}
}

void DemoPlayer::ActorOnCollisionStay(Actor* /*other*/, CollisionInfo* info)
{
	//Console::Log("Hit!");

	/*if (other->GetName() == "Abyss")
	{
		// リセット
		mTransform->mPosition = Vector3(0.0f, 2.0f, -10.0f);
		mTransform->mRotation = Quaternion::kIdentity;
		mVelocity = Vector3::kZero;
	}*/

	float maxCos = cosf(MyMath::ToRadians(mMaxGround));
	//Console::Log(std::format("{}\n", MyMath::ToDegrees(acosf(info->mNormal.y))));
	Vector3 pushBack = info->mNormal;
	if (info->mNormal.y >= maxCos)
	{
		mIsGround = true;
		mVelocity.y = 0.0f;
		pushBack = Vector3(0.0f, 1.0f, 0.0f);
	}
	// 押し戻し
	mTransform->mPosition = mTransform->mPosition + pushBack * info->mDepth;
	mTransform->UpdateWorld(mParent ? mParent->mTransform : nullptr);
}

// ==================================================
// 開発用
// ==================================================

void DemoPlayer::ActorUpdateForDev()
{
	ImGui::Text("This is demo player!");
	ImGui::Text(std::format("Is Ground: {}", mIsGround).c_str());
	ImGui::DragFloat("Jump Power", &mJumpPower, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("Gravity", &mGravity, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("Ground Dist", &mGroundDist, 0.001f, 0.0f, 100.0f);
	ImGui::DragFloat("Max Ground", &mMaxGround, 0.01f, 1.0f, 180.0f);
	ImGui::DragFloat("Cooldown", &mInitCooldown, 0.001f, 0.0f, 1.0f);
}

// ==================================================
// json
// ==================================================

void DemoPlayer::Load(const nlohmann::json& json)
{
	Actor::Load(json);
	JsonHelper::GetFloat(json, "Jump Power", mJumpPower);
	JsonHelper::GetFloat(json, "Gravity", mGravity);
	JsonHelper::GetFloat(json, "Ground Dist", mGroundDist);
	JsonHelper::GetFloat(json, "Max Ground", mMaxGround);
	JsonHelper::GetFloat(json, "Cooldown", mInitCooldown);
}

void DemoPlayer::Save(nlohmann::json& json)
{
	Actor::Save(json);
	JsonHelper::SetFloat(json, "Jump Power", mJumpPower);
	JsonHelper::SetFloat(json, "Gravity", mGravity);
	JsonHelper::GetFloat(json, "Ground Dist", mGroundDist);
	JsonHelper::SetFloat(json, "Max Ground", mMaxGround);
	JsonHelper::SetFloat(json, "Cooldown", mInitCooldown);
}
