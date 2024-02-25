#include "Player.h"
#include "Collision/CollisionManager.h"
#include "Component/MeshRenderer.h"
#include "Component/SphereCollider.h"
#include "Component/SpriteRenderer.h"
#include "Editor.h"
#include "Graphics/Renderer.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"

Player::Player(Scene* scene)
	: Actor(scene)
	, mRadius(1.0f)
	, mVelocity(Vector3::kZero)
	, mSpeed(10.0f)
	, mRotVel(0.0f)
	, mRotSpeed(MyMath::kPiOver2)
	, mRotY(0.0f)
	, mIsGround(false)
	, mJumpPower(0.0f)
	, mGravity(0.0f)
	, mGravityPow(0.0f)
	//, mGravityDir(Vector3(0.0f, -1.0f, 0.0f))
	, mGroundDist(0.3f)
	, mMaxGround(60.0f)
	, mNormal(Vector3::kZero)
	, mCurrNorm(Vector3::kZero)
	, mAttractor(nullptr)
{
	// メッシュ
	auto mr = new MeshRenderer(this);
	mr->SetModel(mScene->GetRenderer()->GetModel("Player.obj"));
	// コライダー
	auto sc = new SphereCollider(this);
	sc->SetAttr(Collider::Allies);// 味方
	sc->SetSphere({ {0.0f,0.0f,0.0f},mRadius });
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
			mGravityPow = mJumpPower;
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
	mRotY += mRotVel * mRotSpeed * deltaTime;

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
	/*Vector3 downDir = Vector3(0.0f, -1.0f, 0.0f) * mTransform->mRotation;// down
	Ray ray = Ray(mTransform->mPosition, mTransform->mPosition + downDir);
	RaycastHit info = {};
	if (mScene->GetCollisionManager()->Raycast(ray, info, Collider::Planet))
	{
		//float dist = Length(info.mPoint - ray.mStart);// 地面までの距離
		float dot = Dot(upDir, info.mNormal);
		if (dot <= cosf(MyMath::ToRadians(mMaxGround)))
		{
			mNormal = info.mNormal;
		}
	}*/

	if (mAttractor)
	{
		Vector3 attractorPos = mAttractor->mTransform->GetWorld().GetTranslation();
		mNormal = Normalize(mTransform->mPosition - attractorPos);
		// ログ
		/*Console::Log(std::format("({:6.3f},{:6.3f},{:6.3f})\n",
			mNormal.x,
			mNormal.y,
			mNormal.z));*/
	}

	if (!mIsGround)
	{
		const float kNormSpeed = 0.1f;
		mCurrNorm = MyMath::Lerp<Vector3>(mCurrNorm, mNormal, kNormSpeed);
		mCurrNorm.Normalize();
	}
	else
	{
		mCurrNorm = mNormal;
	}

	// ==================================================
	// 重力
	// ==================================================
	if (!mIsGround)
	{
		mGravityPow += -mGravity;
	}
	mTransform->mPosition += mNormal * mGravityPow * deltaTime;

	// 地面
	mIsGround = false;
	Ray ray = Ray(mTransform->mPosition, mTransform->mPosition - mNormal);
	RaycastHit info = {};
	Collider::Attribute attr = Collider::Attribute(uint32_t(Collider::kAll) & ~uint32_t(Collider::Allies));// 味方以外
	if (mScene->GetCollisionManager()->Raycast(ray, info, attr))
	{
		// トリガー以外
		if (!info.mCollider->GetIsTrigger())
		{
			float dist = Length(info.mPoint - ray.mStart);
			if (dist <= mRadius + mGroundDist)
			{
				mIsGround = true;
				mGravityPow = 0.0f;
				// 押し戻し
				mTransform->mPosition = info.mPoint + mNormal * mRadius;
			}
		}
	}

	// 姿勢を制御
	Vector3 axis = Cross(upDir, mCurrNorm);
	//float len = Length(axis);
	//Console::Log(std::format("Length: {}\n", len));
	if (Length(axis) > 0.001f)// ？
	{
		axis.Normalize();
		float theta = acosf(Dot(upDir, mCurrNorm));
		//Console::Log(std::format("Theta: {}\n", theta));
		mTransform->mRotation *= Quaternion(axis, theta);
		mTransform->mRotation.Normalize();
	}
	/*Vector3 axis = Cross(Vector3(0.0f, 1.0f, 0.0f), mCurrNorm);
	if (Length(axis) > 0.001f)
	{
		axis.Normalize();
		float theta = acosf(Dot(Vector3(0.0f, 1.0f, 0.0f), mCurrNorm));
		Quaternion q = Quaternion(axis, theta);
		q *= Quaternion(Vector3(0.0f, 1.0f, 0.0f) * q, mRotateY);
		mTransform->mRotation = q;

		Vector3 a = GetAxis(q);
		float t = GetTheta(q);
		Console::Log(std::format("({:6.3f},{:6.3f},{:6.3f}): {}\n",
			a.x, a.y, a.z, t));
	}*/
}

void Player::OnCollision(Actor* /*other*/, CollisionInfo* info)
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

void Player::OnTrigger(Actor* other)
{
	if (other->GetName() == "Attractor")
	{
		mAttractor = other;
	}
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
	float len = 5.0f;
	Vector3 u = Vector3(0.0f, 1.0f, 0.0f) * mTransform->mRotation * len;
	Vector3 r = Vector3(1.0f, 0.0f, 0.0f) * mTransform->mRotation * len;
	Vector3 f = Vector3(0.0f, 0.0f, 1.0f) * mTransform->mRotation * len;
	prim->DrawLine3(mTransform->mPosition, mTransform->mPosition + u, Color::kGreen);
	prim->DrawLine3(mTransform->mPosition, mTransform->mPosition + r, Color::kRed);
	prim->DrawLine3(mTransform->mPosition, mTransform->mPosition + f, Color::kBlue);
	prim->DrawLine3(mTransform->mPosition, mTransform->mPosition - mNormal * 100.0f, Color::kWhite);
	prim->DrawLine3(mTransform->mPosition, mTransform->mPosition - mCurrNorm * 100.0f, Color::kWhite);
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
