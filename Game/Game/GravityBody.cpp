#include "GravityBody.h"
#include "Actor/Actor.h"
#include "Collision/Collision.h"
#include "Collision/CollisionManager.h"
#include "Component/Collider.h"
#include "Editor.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"
#include <format>

#include "Attractor.h"

GravityBody::GravityBody(Actor* owner)
	: Component(owner)
	, mAttractor(nullptr)
	, mMass(1.0f)
	, mForce(Vector3::kZero)
	, mNormal(Vector3::kZero)
	, mCurrNormal(Vector3::kZero)
	, mIsGround(false)
{

}

GravityBody::~GravityBody()
{

}

void GravityBody::Update(float deltaTime)
{
	if (mAttractor)
	{
		mAttractor->Attract(this, deltaTime);
	}

	mOwner->mTransform->mPosition += mForce * deltaTime;

	if (mAttractor)
	{
		Vector3 currUp = Vector3(0.0f, 1.0f, 0.0f) * mOwner->mTransform->mRotation;
		mNormal = Normalize(
			mOwner->mTransform->mPosition -
			mAttractor->GetOwner()->mTransform->mPosition);

		if (mIsGround)
		{
			mCurrNormal = mNormal;
		}
		else
		{
			const float kNormSpeed = 0.1f;
			mCurrNormal = MyMath::Lerp<Vector3>(mCurrNormal, mNormal, kNormSpeed);
			mCurrNormal.Normalize();
		}

		Vector3 axis = Cross(currUp, mCurrNormal);
		if (Length(axis) > 0.001f)
		{
			axis.Normalize();
			float theta = acosf(Dot(currUp, mCurrNormal));
			mOwner->mTransform->mRotation *= Quaternion(axis, theta);
			mOwner->mTransform->mRotation.Normalize();
		}
	}

	mIsGround = false;
	Vector3 dir = mNormal * mOwner->mTransform->mRotation;
	Ray ray = Ray(
		mOwner->mTransform->mPosition,
		mOwner->mTransform->mPosition - dir);
	RaycastHit info = {};
	if (mOwner->GetScene()->GetCollisionManager()->Raycast(ray, info, Collider::Terrain))
	{
		// トリガー以外
		if (!info.mCollider->GetIsTrigger())
		{
			float dist = Length(info.mPoint - ray.mStart);
			if (dist <= 1.1f)
			{
				mIsGround = true;
				mForce = Vector3::kZero;
				mOwner->mTransform->mPosition = info.mPoint + dir * 1.0f;
			}
		}
	}
}

void GravityBody::AddForce(const Vector3& force)
{
	mForce += force;
}

// ==================================================
// json
// ==================================================

void GravityBody::Load(const nlohmann::json& json)
{
	JsonHelper::GetFloat(json, "Mass", mMass);
}

void GravityBody::Save(nlohmann::json& json)
{
	JsonHelper::SetFloat(json, "Mass", mMass);
}

// ==================================================
// 開発用
// ==================================================

void GravityBody::UpdateForDev()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Gravity Body"))
	{
		ImGui::Text(std::format("Is Ground: {}", mIsGround).c_str());
		ImGui::DragFloat("Mass", &mMass, 0.01f, 0.0f, 100.0f);
		ImGui::TreePop();
	}
}
