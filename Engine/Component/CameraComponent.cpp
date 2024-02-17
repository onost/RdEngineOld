#include "CameraComponent.h"
#include "Actor/Actor.h"
#include "Editor.h"
#include "Graphics/Renderer.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"

CameraComponent::CameraComponent(Actor* owner)
	: Component(owner)
	, mCamera(std::make_unique<Camera>())
	, mIsMain(false)
{
	//SetMainCamera();
}

CameraComponent::~CameraComponent()
{

}

void CameraComponent::SetMainCamera()
{
	if (mCamera)
	{
		// メインカメラとしてレンダラーへセット
		//mOwner->GetScene()->GetRenderer()->SetGameCamera(mCamera.get());
		mOwner->GetScene()->GetRenderer()->SetGameCamera(this);
	}
}

void CameraComponent::OnUpdateWorld()
{
	if (mCamera)
	{
		// オーナーと紐づける
		Matrix4 world = mOwner->mTransform->GetWorld();
		mCamera->SetRotation(world.GetRotation());
		mCamera->SetPosition(world.GetTranslation());
	}
}

// ==================================================
// json
// ==================================================

void CameraComponent::Load(const nlohmann::json& json)
{
	Component::Load(json);
	JsonHelper::GetBool(json, "Is Main", mIsMain);
	if (mIsMain)
	{
		SetMainCamera();
	}

	float fov;
	JsonHelper::GetFloat(json, "Fov", fov);
	mCamera->SetFov(fov);
	float nearZ;
	JsonHelper::GetFloat(json, "NearZ", nearZ);
	mCamera->SetNearZ(nearZ);
	float farZ;
	JsonHelper::GetFloat(json, "FarZ", farZ);
	mCamera->SetFarZ(farZ);
}

void CameraComponent::Save(nlohmann::json& json)
{
	Component::Save(json);
	JsonHelper::SetBool(json, "Is Main", mIsMain);

	JsonHelper::SetFloat(json, "Fov", mCamera->GetFov());
	JsonHelper::SetFloat(json, "NearZ", mCamera->GetNearZ());
	JsonHelper::SetFloat(json, "FarZ", mCamera->GetFarZ());
}

// ==================================================
// 開発用
// ==================================================

void CameraComponent::UpdateForDev()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Camera Component"))
	{
		Component::UpdateForDev();
		if (ImGui::Button("Set as main"))
		{
			SetMainCamera();
		}
		// Fov
		float fov = mCamera->GetFov();
		fov = MyMath::ToDegrees(fov);
		if (ImGui::SliderFloat("Fov", &fov, 1.0f, 180.0f))
		{
			fov = MyMath::ToRadians(fov);
			mCamera->SetFov(fov);
		}
		// Near Z, Far Z
		float nearZ = mCamera->GetNearZ();
		float farZ = mCamera->GetFarZ();
		if (ImGui::DragFloat("NearZ", &nearZ, 0.1f, 0.1f, farZ))
		{
			mCamera->SetNearZ(nearZ);
		}
		if (ImGui::DragFloat("FarZ", &farZ, 0.1f, nearZ, 10000.0f))
		{
			mCamera->SetFarZ(farZ);
		}
		ImGui::TreePop();
	}
}
