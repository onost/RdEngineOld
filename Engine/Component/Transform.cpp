#include "Transform.h"
#include "Actor/Actor.h"
#include "Editor.h"
#include "Graphics/Camera.h"
#include "Graphics/Renderer.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"

Transform::Transform(Actor* owner)
	: Component(owner, 0)
	, mScale(Vector3::kOne)
	, mRotation(Quaternion::kIdentity)
	, mPosition(Vector3::kZero)
	, mCBuff(nullptr)
	, mLocal(Matrix4::kIdentity)
	, mWorld(Matrix4::kIdentity)
	, mTWorld(Matrix4::kIdentity)
{
	mCBuff = std::make_unique<ConstantBuffer>();
	mCBuff->Create(sizeof(Matrix4));
}

void Transform::UpdateLocal()
{
	// ローカル行列
	mLocal = Matrix4::CreateAffine(mScale, mRotation, mPosition);
}

void Transform::UpdateWorld(Transform* parent)
{
	UpdateLocal();
	mWorld = mLocal;
	mTWorld = mLocal;
	if (parent)
	{
		// ワールド行列
		mWorld *= parent->GetWorld();
		mTWorld *= parent->GetWorld();
	}
	mCBuff->Copy(mWorld);
}

void Transform::Bind(ID3D12GraphicsCommandList* cmdList, uint32_t rootParamIdx)
{
	//mCBuff->Copy(mWorld);
	mCBuff->Bind(cmdList, rootParamIdx);
}

void Transform::Copy(Matrix4 world)
{
	mCBuff->Copy(world);
}

// ==================================================
// json
// ==================================================

void Transform::Load(const nlohmann::json& json)
{
	JsonHelper::GetVector3(json, "Scale", mScale);
	JsonHelper::GetQuaternion(json, "Rotation", mRotation);
	JsonHelper::GetVector3(json, "Position", mPosition);
}

void Transform::Save(nlohmann::json& json)
{
	JsonHelper::SetVector3(json, "Scale", mScale);
	JsonHelper::SetQuaternion(json, "Rotation", mRotation);
	JsonHelper::SetVector3(json, "Position", mPosition);
}

// ==================================================
// 開発用
// ==================================================

void Transform::UpdateForDev()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Transform"))
	{
		// Scale
		ImGui::DragFloat3("Scale", &mScale.x, 0.01f);
		// Rotation
		auto rotation = ToEuler(mRotation);
		rotation.x = MyMath::ToDegrees(rotation.x);
		rotation.y = MyMath::ToDegrees(rotation.y);
		rotation.z = MyMath::ToDegrees(rotation.z);
		if (ImGui::DragFloat3("Rotation", &rotation.x, 0.5f))
		{
			rotation.x = MyMath::ToRadians(rotation.x);
			rotation.y = MyMath::ToRadians(rotation.y);
			rotation.z = MyMath::ToRadians(rotation.z);
			mRotation = ToQuaternion(rotation);
		}
		// Position
		ImGui::DragFloat3("Position", &mPosition.x, 0.05f);
		ImGui::TreePop();
	}
}

void Transform::RenderForDev(Primitive* /*prim*/)
{
	/*auto renderer = mOwner->GetScene()->GetRenderer();
	auto camera = renderer->GetCurrCamera();
	//auto m = Matrix4::kIdentity;
	ImGuizmo::DrawCubes(&camera->GetView().m[0][0], &camera->GetProj().m[0][0], &mWorld.m[0][0], 1);
	ImGuizmo::Manipulate(&camera->GetView().m[0][0], &camera->GetProj().m[0][0], ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, &mWorld.m[0][0]);*/
}
