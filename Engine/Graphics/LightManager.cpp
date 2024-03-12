#include "LightManager.h"
#include "Editor.h"
#include "Helper/JsonHelper.h"
#include "MyMath.h"
#include <format>

void LightManager::Initialize()
{
	for (uint32_t i = 0; i < kDirectionalLightCount; ++i)
	{
		mDirectionalLights[i].mColor = Color::kWhite;
		mDirectionalLights[i].mDirection = Vector3(0.0f, -1.0f, 0.0f);
		mDirectionalLights[i].mIntensity = 0.0f;
	}
	for (uint32_t i = 0; i < kPointLightCount; ++i)
	{
		mPointLights[i].mColor = Color::kWhite;
		mPointLights[i].mPosition = Vector3::kZero;
		mPointLights[i].mIntensity = 0.0f;
		mPointLights[i].mRadius = 10.0f;
		mPointLights[i].mDecay = 1.0f;
	}
	for (uint32_t i = 0; i < kSpotLightCount; ++i)
	{
		mSpotLights[i].mColor = Color::kWhite;
		mSpotLights[i].mDirection = Vector3(0.0f, -1.0f, 0.0f);
		mSpotLights[i].mIntensity = 0.0f;
		mSpotLights[i].mPosition = Vector3(0.0f, 5.0f, 0.0f);
		mSpotLights[i].mRadius = 10.0f;
		mSpotLights[i].mDecay = 1.0f;
		mSpotLights[i].mInner = cosf(MyMath::ToRadians(20.0f));
		mSpotLights[i].mOuter = cosf(MyMath::ToRadians(30.0f));
	}
	for (uint32_t i = 0; i < kCircleShadowCount; ++i)
	{
		mCircleShadows[i].mDirection = Vector3(0.0f, -1.0f, 0.0f);
		mCircleShadows[i].mIntensity = 0.0f;
		mCircleShadows[i].mPosition = Vector3(0.0f, 5.0f, 0.0f);
		mCircleShadows[i].mRadius = 10.0f;
		mCircleShadows[i].mDecay = 1.0f;
		mCircleShadows[i].mInner = cosf(MyMath::ToRadians(20.0f));
		mCircleShadows[i].mOuter = cosf(MyMath::ToRadians(30.0f));
	}

	mCBuff = std::make_unique<ConstantBuffer>();
	mCBuff->Create(sizeof(Constant));
	//mData = static_cast<Constant*>(mCBuff->GetData());
}

void LightManager::Update()
{
	Constant* data = static_cast<Constant*>(mCBuff->GetData());
	// Directional Light
	for (uint32_t i = 0; i < kDirectionalLightCount; ++i)
	{
		data->mDirectionalLights[i] = mDirectionalLights[i];
		data->mDirectionalLights[i].mDirection.Normalize();// Normalize
	}
	// Point Light
	for (uint32_t i = 0; i < kPointLightCount; ++i)
	{
		data->mPointLights[i] = mPointLights[i];
	}
	// Spot Light
	for (uint32_t i = 0; i < kSpotLightCount; ++i)
	{
		data->mSpotLights[i] = mSpotLights[i];
		data->mSpotLights[i].mDirection.Normalize();
		// Cos
		data->mSpotLights[i].mInner = cosf(MyMath::ToRadians(mSpotLights[i].mInner));
		data->mSpotLights[i].mOuter = cosf(MyMath::ToRadians(mSpotLights[i].mOuter));
	}
	// Circle Shadow
	for (uint32_t i = 0; i < kCircleShadowCount; ++i)
	{
		data->mCircleShadows[i] = mCircleShadows[i];
		data->mCircleShadows[i].mDirection.Normalize();
		// Cos
		data->mCircleShadows[i].mInner = cosf(MyMath::ToRadians(mCircleShadows[i].mInner));
		data->mCircleShadows[i].mOuter = cosf(MyMath::ToRadians(mCircleShadows[i].mOuter));
	}
}

void LightManager::Bind(ID3D12GraphicsCommandList* cmdList, uint32_t lightRootParamIdx)
{
	Update();
	mCBuff->Bind(cmdList, lightRootParamIdx);
}

// ==================================================
// json
// ==================================================

void LightManager::LoadLevel(const nlohmann::json& json)
{
	// Directional Light
	for (uint32_t i = 0; i < kDirectionalLightCount; ++i)
	{
		const nlohmann::json& data = json["Directional Light"];
		JsonHelper::GetColor(data[i], "Color", mDirectionalLights[i].mColor);
		JsonHelper::GetVector3(data[i], "Direction", mDirectionalLights[i].mDirection);
		JsonHelper::GetFloat(data[i], "Intensity", mDirectionalLights[i].mIntensity);
	}
	// Point Light
	for (uint32_t i = 0; i < kPointLightCount; ++i)
	{
		const nlohmann::json& data = json["Point Light"];
		JsonHelper::GetColor(data[i], "Color", mPointLights[i].mColor);
		JsonHelper::GetVector3(data[i], "Position", mPointLights[i].mPosition);
		JsonHelper::GetFloat(data[i], "Intensity", mPointLights[i].mIntensity);
		JsonHelper::GetFloat(data[i], "Radius", mPointLights[i].mRadius);
		JsonHelper::GetFloat(data[i], "Decay", mPointLights[i].mDecay);
	}
	// Spot Light
	for (uint32_t i = 0; i < kSpotLightCount; ++i)
	{
		const nlohmann::json& data = json["Spot Light"];
		JsonHelper::GetColor(data[i], "Color", mSpotLights[i].mColor);
		JsonHelper::GetVector3(data[i], "Direction", mSpotLights[i].mDirection);
		JsonHelper::GetFloat(data[i], "Intensity", mSpotLights[i].mIntensity);
		JsonHelper::GetVector3(data[i], "Position", mSpotLights[i].mPosition);
		JsonHelper::GetFloat(data[i], "Radius", mSpotLights[i].mRadius);
		JsonHelper::GetFloat(data[i], "Decay", mSpotLights[i].mDecay);
		JsonHelper::GetFloat(data[i], "Inner", mSpotLights[i].mInner);
		JsonHelper::GetFloat(data[i], "Outer", mSpotLights[i].mOuter);
	}
	// Circle Shadow
	for (uint32_t i = 0; i < kCircleShadowCount; ++i)
	{
		const nlohmann::json& data = json["Circle Shadow"];
		if (data.size() > i)
		{
			JsonHelper::GetVector3(data[i], "Direction", mCircleShadows[i].mDirection);
			JsonHelper::GetFloat(data[i], "Intensity", mCircleShadows[i].mIntensity);
			JsonHelper::GetVector3(data[i], "Position", mCircleShadows[i].mPosition);
			JsonHelper::GetFloat(data[i], "Radius", mCircleShadows[i].mRadius);
			JsonHelper::GetFloat(data[i], "Decay", mCircleShadows[i].mDecay);
			JsonHelper::GetFloat(data[i], "Inner", mCircleShadows[i].mInner);
			JsonHelper::GetFloat(data[i], "Outer", mCircleShadows[i].mOuter);
		}
	}
}

void LightManager::SaveLevel(nlohmann::json& json)
{
	// Directional Light
	for (uint32_t i = 0; i < kDirectionalLightCount; ++i)
	{
		nlohmann::json& data = json["Directional Light"];
		JsonHelper::SetColor(data[i], "Color", mDirectionalLights[i].mColor);
		JsonHelper::SetVector3(data[i], "Direction", mDirectionalLights[i].mDirection);
		JsonHelper::SetFloat(data[i], "Intensity", mDirectionalLights[i].mIntensity);
	}
	// Point Light
	for (uint32_t i = 0; i < kPointLightCount; ++i)
	{
		nlohmann::json& data = json["Point Light"];
		JsonHelper::SetColor(data[i], "Color", mPointLights[i].mColor);
		JsonHelper::SetVector3(data[i], "Position", mPointLights[i].mPosition);
		JsonHelper::SetFloat(data[i], "Intensity", mPointLights[i].mIntensity);
		JsonHelper::SetFloat(data[i], "Radius", mPointLights[i].mRadius);
		JsonHelper::SetFloat(data[i], "Decay", mPointLights[i].mDecay);
	}
	// Spot Light
	for (uint32_t i = 0; i < kSpotLightCount; ++i)
	{
		nlohmann::json& data = json["Spot Light"];
		JsonHelper::SetColor(data[i], "Color", mSpotLights[i].mColor);
		JsonHelper::SetVector3(data[i], "Direction", mSpotLights[i].mDirection);
		JsonHelper::SetFloat(data[i], "Intensity", mSpotLights[i].mIntensity);
		JsonHelper::SetVector3(data[i], "Position", mSpotLights[i].mPosition);
		JsonHelper::SetFloat(data[i], "Radius", mSpotLights[i].mRadius);
		JsonHelper::SetFloat(data[i], "Decay", mSpotLights[i].mDecay);
		JsonHelper::SetFloat(data[i], "Inner", mSpotLights[i].mInner);
		JsonHelper::SetFloat(data[i], "Outer", mSpotLights[i].mOuter);
	}
	// Circle Shadow
	for (uint32_t i = 0; i < kCircleShadowCount; ++i)
	{
		nlohmann::json& data = json["Circle Shadow"];
		JsonHelper::SetVector3(data[i], "Direction", mCircleShadows[i].mDirection);
		JsonHelper::SetFloat(data[i], "Intensity", mCircleShadows[i].mIntensity);
		JsonHelper::SetVector3(data[i], "Position", mCircleShadows[i].mPosition);
		JsonHelper::SetFloat(data[i], "Radius", mCircleShadows[i].mRadius);
		JsonHelper::SetFloat(data[i], "Decay", mCircleShadows[i].mDecay);
		JsonHelper::SetFloat(data[i], "Inner", mCircleShadows[i].mInner);
		JsonHelper::SetFloat(data[i], "Outer", mCircleShadows[i].mOuter);
	}
}

// ==================================================
// 開発用
// ==================================================

void LightManager::UpdateForDev()
{
	//ImGui::Separator();
	// Directional Light
	for (uint32_t i = 0; i < kDirectionalLightCount; ++i)
	{
		if (ImGui::TreeNode(std::format("Directional Light [ {} ]", i).c_str()))
		{
			ImGui::ColorEdit4("Color", &mDirectionalLights[i].mColor.r);
			ImGui::DragFloat3("Direction", &mDirectionalLights[i].mDirection.x, 0.01f);
			ImGui::DragFloat("Intensity", &mDirectionalLights[i].mIntensity, 0.001f, 0.0f, 100.0f);
			ImGui::TreePop();
		}
	}
	ImGui::Separator();
	// Point Light
	for (uint32_t i = 0; i < kPointLightCount; ++i)
	{
		if (ImGui::TreeNode(std::format("Point Light [ {} ]", i).c_str()))
		{
			ImGui::ColorEdit4("Color", &mPointLights[i].mColor.r);
			ImGui::DragFloat3("Position", &mPointLights[i].mPosition.x, 0.01f);
			ImGui::DragFloat("Intensity", &mPointLights[i].mIntensity, 0.001f, 0.0f, 100.0f);
			ImGui::DragFloat("Radius", &mPointLights[i].mRadius, 0.01f, 0.0f, 1000.0f);
			ImGui::DragFloat("Decay", &mPointLights[i].mDecay, 0.001f, 0.001f, 10.0f);
			ImGui::TreePop();
		}
	}
	ImGui::Separator();
	// Spot Light
	for (uint32_t i = 0; i < kSpotLightCount; ++i)
	{
		if (ImGui::TreeNode(std::format("Spot Light [ {} ]", i).c_str()))
		{
			ImGui::ColorEdit4("Color", &mSpotLights[i].mColor.r);
			ImGui::DragFloat3("Direction", &mSpotLights[i].mDirection.x, 0.01f);
			ImGui::DragFloat("Intensity", &mSpotLights[i].mIntensity, 0.001f, 0.0f, 100.0f);
			ImGui::DragFloat3("Position", &mSpotLights[i].mPosition.x, 0.01f);
			ImGui::DragFloat("Radius", &mSpotLights[i].mRadius, 0.01f, 0.0f, 1000.0f);
			ImGui::DragFloat("Decay", &mSpotLights[i].mDecay, 0.001f, 0.001f, 10.0f);
			ImGui::DragFloat("Inner", &mSpotLights[i].mInner, 0.01f, 0.0f, mSpotLights[i].mOuter);
			ImGui::DragFloat("Outer", &mSpotLights[i].mOuter, 0.01f, mSpotLights[i].mInner, 180.0f);
			ImGui::TreePop();
		}
	}
	ImGui::Separator();
	// Spot Light
	for (uint32_t i = 0; i < kCircleShadowCount; ++i)
	{
		if (ImGui::TreeNode(std::format("Circle Shadow [ {} ]", i).c_str()))
		{
			ImGui::DragFloat3("Direction", &mCircleShadows[i].mDirection.x, 0.01f);
			ImGui::DragFloat("Intensity", &mCircleShadows[i].mIntensity, 0.001f, 0.0f, 100.0f);
			ImGui::DragFloat3("Position", &mCircleShadows[i].mPosition.x, 0.01f);
			ImGui::DragFloat("Radius", &mCircleShadows[i].mRadius, 0.01f, 0.0f, 1000.0f);
			ImGui::DragFloat("Decay", &mCircleShadows[i].mDecay, 0.001f, 0.001f, 10.0f);
			ImGui::DragFloat("Inner", &mCircleShadows[i].mInner, 0.01f, 0.0f, mCircleShadows[i].mOuter);
			ImGui::DragFloat("Outer", &mCircleShadows[i].mOuter, 0.01f, mCircleShadows[i].mInner, 180.0f);
			ImGui::TreePop();
		}
	}
	ImGui::Separator();
}
