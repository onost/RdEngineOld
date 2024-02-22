#include "Player.h"
#include "Component/MeshRenderer.h"
#include "Collision/CollisionManager.h"
#include "Editor.h"
#include "Graphics/Renderer.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"

Player::Player(Scene* scene)
	: Actor(scene)
	, mRadius(1.0f)
	, mVelocity(Vector3::kZero)
	, mSpeed(7.5f)
	, mRotVel(0.0f)
	, mRotSpeed(MyMath::kPiOver2)
	, mIsGround(false)
	, mJumpPower(0.0f)
	, mGravity(0.0f)
	, mGravityPower(0.0f)
	, mGravityDir(Vector3::kZero)
	, mMaxGround(60.0f)
	, mNormal(Vector3::kZero)
{
	// メッシュ
	auto mr = new MeshRenderer(this);
	mr->SetModel(mScene->GetRenderer()->GetModel("Player.obj"));
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
	if (mIsGround)
	{
		if (input.mGamepad.GetButtonDown(XINPUT_GAMEPAD_A) ||
			input.mKeyboard.GetKeyDown(DIK_SPACE))
		{
			//mVelocity.y = mJumpPower;
			mGravityPower = mJumpPower;
		}
	}
}

void Player::ActorUpdate(float deltaTime)
{
	// ==================================================
	// 回転
	// ==================================================
	Vector3 upDir = Vector3(0.0f, 1.0f, 0.0f) * mTransform->mRotation;// 上
	Quaternion rot = Quaternion(upDir, mRotVel * mRotSpeed * deltaTime);
	mTransform->mRotation *= rot;

	// ==================================================
	// 移動
	// ==================================================
	Vector3 r = Vector3(1.0f, 0.0f, 0.0f) * mTransform->mRotation;// right
	Vector3 f = Vector3(0.0f, 0.0f, 1.0f) * mTransform->mRotation;// forward
	mTransform->mPosition += r * mVelocity.x * deltaTime;
	mTransform->mPosition += f * mVelocity.z * deltaTime;

	// ==================================================
	// 法線
	// ==================================================
	Vector3 downDir = Vector3(0.0f, -1.0f, 0.0f) * mTransform->mRotation;// 下
	Ray ray = Ray(mTransform->mPosition, mTransform->mPosition + downDir);
	RaycastHit info = {};
	Collider::Attribute attr = Collider::Attribute(uint32_t(Collider::kAll) & ~uint32_t(Collider::Allies));// 味方以外
	if (mScene->GetCollisionManager()->Raycast(ray, info, attr))
	{
		//float dist = Length(info.mPoint - ray.mStart);
		float dot = Dot(upDir, info.mNormal);
		if (acosf(dot) <= MyMath::ToRadians(mMaxGround))
		{
			mNormal = info.mNormal;
		}
	}

	// ==================================================
	// 重力
	// ==================================================
	if (!mIsGround)
	{
		mGravityPower += -mGravity;
	}
	Vector3 fallDir = Normalize(-mNormal + mGravityDir);
	mTransform->mPosition += -fallDir * mGravityPower * deltaTime;

	// 地面
	mIsGround = false;
	ray = Ray(mTransform->mPosition, mTransform->mPosition + fallDir);// 下へ
	if (mScene->GetCollisionManager()->Raycast(ray, info, attr))
	{
		float dist = Length(info.mPoint - ray.mStart);
		if (dist <= mRadius + mGroundDist)
		{
			mIsGround = true;
			mGravityPower = 0.0f;
			// 押し戻し
			mTransform->mPosition = info.mPoint + Vector3(0.0f, mRadius, 0.0f) * mTransform->mRotation;

			mNormal = info.mNormal;
		}
	}

	{
		Console::Log(std::format("({:6.3f},{:6.3f},{:6.3f})\n",
			mNormal.x,
			mNormal.y,
			mNormal.z));

		// http://marupeke296.com/DXG_No16_AttitudeControl.html
		//Vector3 axis = Cross(Vector3(0.0f, 1.0f, 0.0f), mActualNorm);
		Vector3 axis = Cross(upDir, mNormal);
		if (Length(axis) > 0.001f)
		{
			axis.Normalize();
			//float theta = acosf(Dot(Vector3(0.0f, 1.0f, 0.0f), mActualNorm));
			float theta = acosf(Dot(upDir, mNormal));
			Quaternion q = Quaternion(axis, theta);
			mTransform->mRotation *= q;
		}
	}

	if (!mIsGround && mGravityAttractor)
	{
		Vector3 v = mGravityAttractor->mTransform->GetWorld().GetTranslation() - mTransform->mPosition;
		mTransform->mPosition += Normalize(v) / 10.0f;
	}
}

void Player::OnCollision(Actor* other, CollisionInfo* info)
{
	/*float maxCos = cosf(MyMath::ToRadians(45.0f));
	//Console::Log(std::format("{}\n", MyMath::ToDegrees(acosf(info->mNormal.y))));
	if (info->mNormal.y >= maxCos)
	{
		mIsGround = true;
		mGravityPower = 0.0f;
		// 押し戻し
		mTransform->mPosition = mTransform->mPosition + Vector3(0.0f, 1.0f, 0.0f) * info->mDepth;
	}
	else
	{
		// 押し戻し
		mTransform->mPosition = mTransform->mPosition + info->mNormal * info->mDepth;
	}*/

	if (other->GetName() == "Gravity Attractor")
	{
		mGravityAttractor = other;
	}
	else
	{
		// 押し戻し
		mTransform->mPosition = mTransform->mPosition + info->mNormal * info->mDepth;
	}

	//Console::Log("Hit!");
	mTransform->UpdateWorld(mParent ? mParent->mTransform : nullptr);
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
}

void Player::Save(nlohmann::json& json)
{
	Actor::Save(json);
	JsonHelper::SetFloat(json, "Jump Power", mJumpPower);
	JsonHelper::SetFloat(json, "Gravity", mGravity);
	JsonHelper::SetFloat(json, "Ground Dist", mGroundDist);
}
