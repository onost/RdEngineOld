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
	, mCurrAtt(nullptr)
	, mMass(1.0f)
	, mForce(0.0f)
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
	if (mCurrAtt)
	{
		mCurrAtt->Attract(this, deltaTime);
	}

	mOwner->mTransform->mPosition += mNormal * mForce * deltaTime;

	//float distance = 0.0f;
	mIsGround = false;
	Ray ray = Ray(
		mOwner->mTransform->mPosition,
		mOwner->mTransform->mPosition - mNormal);
	RaycastHit info = {};
	if (mOwner->GetScene()->GetCollisionManager()->Raycast(ray, info, Collider::Terrain))
	{
		// トリガー以外
		if (!info.mCollider->GetIsTrigger())
		{
			float distance = Length(info.mPoint - ray.mStart);
			if (distance <= 1.1f)
			{
				mIsGround = true;
				mForce = 0.0f;
				mOwner->mTransform->mPosition = info.mPoint + mNormal * 1.0f;
			}
		}
	}

	if (mCurrAtt)
	{
		Vector3 currUp = Vector3(0.0f, 1.0f, 0.0f) * mOwner->mTransform->mRotation;
		mNormal = Normalize(
			mOwner->mTransform->mPosition -
			mCurrAtt->GetOwner()->mTransform->GetWorld().GetTranslation());

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
}

void GravityBody::OnTrigger(Actor* other)
{
	if (other->GetName() == "Attractor")
	{
		auto component = other->GetComponent(Component::Type::Attractor);
		auto attractor = dynamic_cast<Attractor*>(component);
		mCurrAtt = attractor;
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

void GravityBody::RenderForDev(Primitive* prim)
{
	Vector3 pos = mOwner->mTransform->mPosition;
	const float kLen = 100.0f;
	prim->DrawLine3(pos, pos - mNormal * kLen, Color::kWhite);
	prim->DrawLine3(pos, pos - mCurrNormal * kLen, Color::kWhite);
}
