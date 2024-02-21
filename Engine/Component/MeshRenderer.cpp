#include "MeshRenderer.h"
#include "Actor/Actor.h"
#include "Editor.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Renderer.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"

MeshRenderer::MeshRenderer(Actor* owner)
	: RendererComponent(owner)
	, mModel(nullptr)
	, mShaderType(ModelCommon::Type::Default)
	, mModelName()
	, mMatForDev(nullptr)
	, mTexturePath()
{
	mOwner->GetScene()->GetRenderer()->AddMesh(this);
}

MeshRenderer::~MeshRenderer()
{
	mOwner->GetScene()->GetRenderer()->RemoveMesh(this);
}

void MeshRenderer::Draw()
{
	if (mIsVisible && mModel)
	{
		//mModel->Draw(mOwner->mTransform, mShaderType);
		MyAssert(ModelCommon::mCmdList);
		ModelCommon::SetPso(mShaderType);
		for (auto& mesh : mModel->GetMeshes())
		{
			mOwner->mTransform->Copy(mesh->GetLocal() * mOwner->mTransform->GetWorld());
			mOwner->mTransform->Bind(ModelCommon::mCmdList, 0);
			mesh->Draw(ModelCommon::mCmdList, 2, 3);
		}
	}
}

// ==================================================
// json
// ==================================================

void MeshRenderer::Load(const nlohmann::json& json)
{
	RendererComponent::Load(json);
	// Shader Type
	uint32_t shaderType = 0;
	JsonHelper::GetUint(json, "Shader Type", shaderType);
	mShaderType = ModelCommon::Type(shaderType);
	// Model
	std::string modelName;
	if (JsonHelper::GetString(json, "Model", modelName))
	{
		auto renderer = mOwner->GetScene()->GetRenderer();
		auto model = renderer->GetModel(modelName);
		SetModel(model);
	}
	// Texture
	std::string texturePath;
	if (JsonHelper::GetString(json, "Texture", texturePath))
	{
		auto renderer = mOwner->GetScene()->GetRenderer();
		auto texture = renderer->GetTexture(texturePath);
		if (texture)
		{
			mMatForDev->SetTexture(texture);
			mTexturePath = texture->GetPath();
		}
	}
}

void MeshRenderer::Save(nlohmann::json& json)
{
	RendererComponent::Save(json);
	JsonHelper::SetUint(json, "Shader Type", uint32_t(mShaderType));
	if (mModel)
	{
		JsonHelper::SetString(json, "Model", mModel->GetName());
	}
	auto texture = mMatForDev->GetTexture();
	if (texture)
	{
		JsonHelper::SetString(json, "Texture", texture->GetPath());
	}
}

// ==================================================
// 開発用
// ==================================================

void MeshRenderer::UpdateForDev()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Mesh Renderer"))
	{
		UpdateModel();
		ImGui::TreePop();
	}
}

void MeshRenderer::UpdateModel()
{
	RendererComponent::UpdateForDev();
	// Shader Type
	Editor::Combo("Shader Type", mShaderType);
	// Model Name
	ImGui::InputText("Model Name", &mModelName);
	if (ImGui::BeginDragDropTarget())// ドロップ
	{
		if (auto payload = ImGui::AcceptDragDropPayload("MODEL_PAYLOAD"))
		{
			auto model = *(Model**)(payload->Data);
			SetModel(model);
		}
		ImGui::EndDragDropTarget();
	}
	if (ImGui::Button("Load##Model"))
	{
		auto renderer = mOwner->GetScene()->GetRenderer();
		auto model = renderer->GetModel(mModelName);
		SetModel(model);
	}
	// Material
	if (mModel)
	{
		auto& mats = mModel->GetMaterials();
		/*if (!mMaterial && mats.size() > 0)
		{
			mMaterial = mats.begin()->second;
		}*/
		if (mMatForDev)
		{
			// Combo
			if (ImGui::BeginCombo("Material", mMatForDev->GetName().c_str()))
			{
				auto it = mats.begin();
				for (; it != mats.end(); ++it)
				{
					auto m = it->second;
					if (ImGui::Selectable(m->GetName().c_str(), mMatForDev == m))
					{
						mMatForDev = m;
					}
					if (mMatForDev == m)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			// Base Color
			Color color = mMatForDev->GetBaseColor();
			if (ImGui::ColorEdit4("Base Color", &color.r))
			{
				mMatForDev->SetBaseColor(color);
			}
			// Specular
			float spec = mMatForDev->GetSpecular();
			if (ImGui::DragFloat("Specular", &spec, 0.1f, 0.0f, MyMath::kInfinity))
			{
				mMatForDev->SetSpecular(spec);
			}
			// Is Lighting
			/*bool isLighting = mMatForDev->GetIsLighting();
			if (ImGui::Checkbox("Is Lighting", &isLighting))
			{
				mMatForDev->SetIsLighting(isLighting);
			}*/

			// Texture
			ImGui::InputText("Texture Path", &mTexturePath);
			if (ImGui::BeginDragDropTarget())// ドロップ
			{
				if (auto payload = ImGui::AcceptDragDropPayload("TEXTURE_PAYLOAD"))
				{
					auto texture = (Texture**)(payload->Data);
					mMatForDev->SetTexture(*texture);
					mTexturePath = (*texture)->GetPath();
				}
				ImGui::EndDragDropTarget();
			}
			if (ImGui::Button("Load"))
			{
				auto renderer = mOwner->GetScene()->GetRenderer();
				auto texture = renderer->GetTexture(mTexturePath);
				if (texture)
				{
					mMatForDev->SetTexture(texture);
				}
			}
		}
		else
		{
			ImGui::Text("Material not found.");
		}
	}
	else
	{
		ImGui::Text("Model not found.");
	}
}

// ==================================================
// ヘルパー関数
// ==================================================

void MeshRenderer::SetModel(Model* model)
{
	if (model)
	{
		mModel = model;
		mModelName = mModel->GetName();
		auto& mats = mModel->GetMaterials();
		if (mats.size() > 0)
		{
			mMatForDev = mats.begin()->second;
			mTexturePath = mMatForDev->GetTexture()->GetPath();
		}
	}
}
