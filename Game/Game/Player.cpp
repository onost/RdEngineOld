#include "Player.h"
#include "Collision/CollisionManager.h"
#include "Component/MeshRenderer.h"
#include "Component/SphereCollider.h"
#include "Component/SpriteRenderer.h"
#include "Editor.h"
#include "Graphics/Renderer.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"

#include "GravityBody.h"
#include "Attractor.h"

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
	, mGravityBody(nullptr)
{
	// メッシュ
	auto mr = new MeshRenderer(this);
	mr->SetModel(mScene->GetRenderer()->GetModel("Player.obj"));
	// コライダー
	auto sc = new SphereCollider(this);
	sc->SetAttr(Collider::Allies);// 味方
	sc->SetSphere({ {0.0f,0.0f,0.0f},mRadius });

	mGravityBody = new GravityBody(this);
}

void Player::ActorInput(const Input::State& input)
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
}

void Player::ActorOnCollision(Actor* /*other*/, CollisionInfo* info)
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

void Player::ActorOnTrigger(Actor* other)
{
	// ゴール！
	if (other->GetName() == "GoalFlag")
	{
		auto actor = mScene->GetActor("GoalSprite");
		auto component = actor->GetComponent(Component::Type::SpriteRenderer);
		SpriteRenderer* sprite = dynamic_cast<SpriteRenderer*>(component);
		if (sprite)
		{
			sprite->SetIsVisible(true);
		}
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
