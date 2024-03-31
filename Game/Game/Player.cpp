#include "Player.h"
#include "Collision/CollisionManager.h"
#include "Component/MeshRenderer.h"
#include "Component/SphereCollider.h"
#include "Component/SpriteRenderer.h"
#include "Editor.h"
#include "Graphics/Renderer.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"
#include "RdEngine.h"

#include "GravityBody.h"
#include "Attractor.h"
#include "Component/SkinnedMeshRenderer.h"

Player::Player(Scene* scene)
	: Actor(scene)
	, mRadius(1.0f)
	, mVelocity(Vector3::kZero)
	, mSpeed(10.0f)
	, mRotVel(0.0f)
	, mRotSpeed(MyMath::kPiOver2)
	, mIsGround(false)
	, mJumpPower(0.0f)
	, mGravity(0.0f)
	, mGravityPow(0.0f)
	, mGroundDist(0.3f)
	, mMaxGround(60.0f)
	, mHp(3)
	, mInvincibleTimer(0.0f)
	, mDamage(nullptr)
	, mRenderer(nullptr)
{
	// コライダー
	auto sc = new SphereCollider(this);
	sc->SetAttribute(CollisionAttr::Allies);// 味方
	sc->SetSphere({ {0.0f,0.0f,0.0f},mRadius });

	mGravityBody = new GravityBody(this);
}

void Player::Initialize()
{
	// プレイヤーのHP
	for (uint32_t i = 0; i < 3; ++i)
	{
		mHpUI[i] = Instantiate("PlayerLife");
		mHpUI[i]->mTransform->mPosition = Vector3(20.0f + i * 80.0f, 20.0f, 0.0f);
	}

	auto component = GetComponent(Component::Type::SkinnedMeshRenderer);
	auto renderer = dynamic_cast<SkinnedMeshRenderer*>(component);
	if (renderer)
	{
		auto mat = renderer->GetModel()->GetMaterials();
		for (auto m : mat)
		{
			m.second->SetIsShadowCast(false);
		}
	}

	auto actor = mScene->GetActor("GameOverSprite");
	if (actor)
	{
		component = actor->GetComponent(Component::Type::SpriteRenderer);
		mGameOverSprite = dynamic_cast<SpriteRenderer*>(component);
	}
	actor = mScene->GetActor("GoalSprite");
	if (actor)
	{
		component = actor->GetComponent(Component::Type::SpriteRenderer);
		mGoalSprite = dynamic_cast<SpriteRenderer*>(component);
	}
}

void Player::ActorInput(const InputSystem::State& input)
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
	if (mGravityBody->GetIsGround())
	{
		if (input.mGamepad.GetButtonDown(XINPUT_GAMEPAD_A) ||
			input.mKeyboard.GetKeyDown(DIK_SPACE))
		{
			mGravityBody->SetForce(mJumpPower);
		}
	}
}

void Player::ActorUpdate(float deltaTime)
{
	if (!mDamage)
	{
		auto actor = mScene->GetActor("DamageSprite");
		if (actor)
		{
			auto component = actor->GetComponent(Component::Type::SpriteRenderer);
			mDamage = dynamic_cast<SpriteRenderer*>(component);
		}
	}
	if (!mRenderer)
	{
		auto component = GetComponent(Component::Type::SkinnedMeshRenderer);
		mRenderer = dynamic_cast<SkinnedMeshRenderer*>(component);
	}

	// ==================================================
	// 回転
	// ==================================================
	Vector3 upDir = Vector3(0.0f, 1.0f, 0.0f) * mTransform->mRotation;// up
	Quaternion rot = Quaternion(upDir, mRotVel * mRotSpeed * deltaTime);
	mTransform->mRotation *= rot;

	// ==================================================
	// 移動
	// ==================================================
	Vector3 r = Vector3(1.0f, 0.0f, 0.0f) * mTransform->mRotation;// right
	Vector3 f = Vector3(0.0f, 0.0f, 1.0f) * mTransform->mRotation;// forward
	mTransform->mPosition += r * mVelocity.x * deltaTime;
	mTransform->mPosition += f * mVelocity.z * deltaTime;
	if (mRenderer)
	{
		if (!mGravityBody->GetIsGround())
		{
			if (mRenderer->GetAnimation()->GetName() != "Jump.gltf/Armature|mixamo.com|Layer0.001")
			{
				mRenderer->PlayAnimation(mScene->GetRenderer()->GetAnimation("Jump.gltf/Armature|mixamo.com|Layer0.001"));
				mRenderer->SetIsLoop(false);
			}
		}
		else if (Length(mVelocity) > 0.0f && mIsGround)
		{
			if (mRenderer->GetAnimation()->GetName() != "Astronaut.gltf/Armature|mixamo.com|Layer0")
			{
				mRenderer->PlayAnimation(mScene->GetRenderer()->GetAnimation("Astronaut.gltf/Armature|mixamo.com|Layer0"));
				mRenderer->SetIsLoop(true);
			}
		}
		else
		{
			if (mRenderer->GetAnimation()->GetName() != "Idle.gltf/Armature|mixamo.com|Layer0")
			{
				mRenderer->PlayAnimation(mScene->GetRenderer()->GetAnimation("Idle.gltf/Armature|mixamo.com|Layer0"));
				mRenderer->SetIsLoop(true);
			}
		}
	}

	auto light = mScene->GetRenderer()->GetLightManager();
	auto normal = mGravityBody->GetCurrNormal();
	light->SetCircleShadowDirection(0, -normal);
	light->SetCircleShadowPosition(0, mTransform->GetWorld().GetTranslation() + normal * 10.0f);
	light->SetCircleShadowIntensity(0, 0.5f);

	if (mHp <= 0)
	{
		/*auto actor = mScene->GetActor("GameOverSprite");
		if (actor)
		{
			auto component = actor->GetComponent(Component::Type::SpriteRenderer);
			SpriteRenderer* sprite = dynamic_cast<SpriteRenderer*>(component);
			if (sprite)
			{
				sprite->SetIsVisible(true);
			}
		}*/
		if (!mGoalSprite->GetIsVisible())
		{
			mGameOverSprite->SetIsVisible(true);
		}
	}

	mInvincibleTimer = MyMath::Max(0.0f, mInvincibleTimer - deltaTime);
	if (mDamage)
	{
		if (mInvincibleTimer > 0.0f)
		{
			auto color = mDamage->GetColor();
			if ((mInvincibleTimer <= 2.0f && mInvincibleTimer > 1.9f))
			{
				color.a += 0.2f;
			}
			else
			{
				color.a -= 0.05f;
			}
			mDamage->SetColor(color);

			if (std::fmodf(mInvincibleTimer, 0.2f) > 0.1f)
			{
				mRenderer->SetIsVisible(true);
			}
			else
			{
				mRenderer->SetIsVisible(false);
			}
		}
		else
		{
			auto color = mDamage->GetColor();
			color.a = 0.0f;
			mDamage->SetColor(color);
			mRenderer->SetIsVisible(true);
		}
	}
}

void Player::ActorOnCollisionStay(Actor* /*other*/, CollisionInfo* info)
{
	//Console::Log("Hit!");

	float maxCos = cosf(MyMath::ToRadians(mMaxGround));
	//Console::Log(std::format("{}\n", MyMath::ToDegrees(acosf(info->mNormal.y))));
	Vector3 pushBack = info->mNormal;
	if (info->mNormal.y >= maxCos)
	{
		mIsGround = true;
		mGravityPow = 0.0f;
		//pushBack = Vector3(0.0f, 1.0f, 0.0f);
	}
	// 押し戻し
	mTransform->mPosition = mTransform->mPosition + pushBack * info->mDepth;
	mTransform->UpdateWorld(mParent ? mParent->mTransform : nullptr);
}

void Player::ActorOnTriggerEnter(Actor* other)
{
	// ゴール！
	if (other->GetName() == "GoalFlag" && mHp > 0)
	{
		/*auto actor = mScene->GetActor("GoalSprite");
		auto component = actor->GetComponent(Component::Type::SpriteRenderer);
		SpriteRenderer* sprite = dynamic_cast<SpriteRenderer*>(component);
		if (sprite)
		{
			sprite->SetIsVisible(true);
		}*/
		if (!mGameOverSprite->GetIsVisible())
		{
			mGoalSprite->SetIsVisible(true);
		}
	}
}

void Player::ActorOnCollisionEnter(Actor* other, CollisionInfo*)
{
	if (other->GetName() == "Meteorite" && mInvincibleTimer <= 0.0f)
	{
		if (mHp <= 3 && mHp > 0)
		{
			auto comp = mHpUI[mHp - 1]->GetComponent(Component::Type::SpriteRenderer);
			if (comp)
			{
				auto sprite = dynamic_cast<SpriteRenderer*>(comp);
				if (sprite)
				{
					sprite->SetIsVisible(false);
				}
			}

			if (mHp > 1)
			{
				mInvincibleTimer = 2.0f;
			}

			auto audio = gEngine->GetAudioSystem();
			auto data = audio->Load("Assets/Audio/Damage.wav");
			audio->Play(data);
		}
		--mHp;
	}
}

// ==================================================
// 開発用
// ==================================================

void Player::ActorUpdateForDev()
{
	ImGui::Text("This is demo player!");
	ImGui::Text(std::format("Is Ground: {}", mIsGround).c_str());
	ImGui::DragFloat("Jump Power", &mJumpPower, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("Gravity", &mGravity, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("Ground Dist", &mGroundDist, 0.001f, 0.0f, 100.0f);
	ImGui::DragFloat("Max Ground", &mMaxGround, 0.01f, 1.0f, 180.0f);
}

void Player::ActorRenderForDev(Primitive* prim)
{
	static const float kLen = 10.0f;
	Vector3 u = Vector3(0.0f, 1.0f, 0.0f) * mTransform->mRotation * kLen;
	Vector3 r = Vector3(1.0f, 0.0f, 0.0f) * mTransform->mRotation * kLen;
	Vector3 f = Vector3(0.0f, 0.0f, 1.0f) * mTransform->mRotation * kLen;
	prim->DrawLine3(mTransform->mPosition, mTransform->mPosition + f, Color::kBlue);
	prim->DrawLine3(mTransform->mPosition, mTransform->mPosition + r, Color::kRed);
	prim->DrawLine3(mTransform->mPosition, mTransform->mPosition + u, Color::kGreen);
}

// ==================================================
// json
// ==================================================

void Player::Load(const nlohmann::json& json)
{
	Actor::Load(json);
	JsonHelper::GetFloat(json, "Jump Power", mJumpPower);
	JsonHelper::GetFloat(json, "Gravity", mGravity);
	JsonHelper::GetFloat(json, "Ground Dist", mGroundDist);
	JsonHelper::GetFloat(json, "Max Ground", mMaxGround);
}

void Player::Save(nlohmann::json& json)
{
	Actor::Save(json);
	JsonHelper::SetFloat(json, "Jump Power", mJumpPower);
	JsonHelper::SetFloat(json, "Gravity", mGravity);
	JsonHelper::GetFloat(json, "Ground Dist", mGroundDist);
	JsonHelper::SetFloat(json, "Max Ground", mMaxGround);
}
