#include "GravityBody.h"
#include "Actor/Actor.h"
#include "Collision/Collision.h"
#include "Collision/CollisionManager.h"
#include "Component/Collider.h"
#include "Editor.h"
#include "Graphics/Primitive.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"
#include <format>

#include "Attractor.h"

GravityBody::GravityBody(Actor* owner)
	: Component(owner)
	, mAttractor(nullptr)
	, mMass(1.0f)
	, mForce(0.0f)
	, mNormal(Vector3(0.0f, 1.0f, 0.0f))
	, mIsGround(false)
	, mPushDist(1.0f)
	, mGroundDist(0.3f)
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

	mOwner->mTransform->mPosition += mNormal * mForce * deltaTime;

	//float distance = 0.0f;
	mIsGround = false;
	Ray ray = Ray(
		mOwner->mTransform->mPosition,
		mOwner->mTransform->mPosition - mNormal);
	RaycastInfo info = {};
	if (mOwner->GetScene()->GetCollisionManager()->Raycast(ray, info, CollisionAttr::Terrain))
	{
		// トリガー以外
		if (!info.mCollider->GetIsTrigger())
		{
			float distance = Length(info.mPoint - ray.mStart);
			if (distance <= mPushDist + mGroundDist)
			{
				mIsGround = true;
				mForce = 0.0f;
				mOwner->mTransform->mPosition = info.mPoint + mNormal * mPushDist;

				//mCurrNormal = info.mNormal;
			}
		}
	}

	if (mAttractor)
	{
		Vector3 currUp = Vector3(0.0f, 1.0f, 0.0f) * mOwner->mTransform->mRotation;
		mNormal = Normalize(
			mOwner->mTransform->mPosition -
			mAttractor->GetOwner()->mTransform->GetWorld().GetTranslation());

		if (mIsGround)
		{
			mCurrNormal = mNormal;
		}
		else
		{
			const float kNormSpeed = 0.2f;
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
}

void GravityBody::OnTriggerEnter(Actor* other)
{
	if (other->GetName() == "Attractor")
	{
		if (!mIsGround)
		{
			auto component = other->GetComponent(Component::Type::Attractor);
			auto attractor = dynamic_cast<Attractor*>(component);
			mAttractor = attractor;
		}
	}
}

void GravityBody::AddForce(float force)
{
	mForce += force;
}

// ==================================================
// json
// ==================================================

void GravityBody::Load(const nlohmann::json& json)
{
	JsonHelper::GetFloat(json, "Mass", mMass);
	JsonHelper::GetFloat(json, "Push Dist", mPushDist);
	JsonHelper::GetFloat(json, "Ground Dist", mGroundDist);
	JsonHelper::GetVector3(json, "Normal", mNormal);
}

void GravityBody::Save(nlohmann::json& json)
{
	JsonHelper::SetFloat(json, "Mass", mMass);
	JsonHelper::SetFloat(json, "Push Dist", mPushDist);
	JsonHelper::SetFloat(json, "Ground Dist", mGroundDist);
	JsonHelper::SetVector3(json, "Normal", mNormal);
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
		ImGui::DragFloat("Push Dist", &mPushDist, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("Ground Dist", &mGroundDist, 0.01f, 0.0f, 100.0f);
		ImGui::TreePop();
	}
}

void GravityBody::RenderForDev(Primitive* prim)
{
	Vector3 pos = mOwner->mTransform->mPosition;
	const float kLen = 100.0f;
	prim->DrawLine3(pos, pos - mNormal * kLen, Color::kWhite);
}
