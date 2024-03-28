#include "ParticleRenderer.h"
#include "Actor/Actor.h"
#include "Editor.h"
#include "Graphics/Renderer.h"
#include "Helper/JsonHelper.h"
#include "Loader/ObjLoader.h"
#include "Scene/Scene.h"

ParticleRenderer::ParticleRenderer(Actor* owner)
	: RendererComponent(owner)
	, mBlend(ParticleCommon::Blend::Normal)
	, mParticle(nullptr)
	, mPlane(nullptr)
	, mTexturePath()
{
	mParticle = std::make_unique<ParticleManager>();
	mParticle->Create();
	// モデルを作成
	mPlane.reset(ObjLoader::Load("Plane.obj"));
	/*if (mPlane)
	{
		auto mat = mPlane->GetMaterials().find("Effect")->second;
		mat->SetIsLighting(false);
	}*/

	mOwner->GetScene()->GetRenderer()->AddParticle(this);
}

ParticleRenderer::~ParticleRenderer()
{
	mOwner->GetScene()->GetRenderer()->RemoveParticle(this);
}

void ParticleRenderer::Update(float deltaTime)
{
	auto position = mOwner->mTransform->GetWorld().GetTranslation();
	mParticle->SetPosition(position);
	mParticle->Update(deltaTime);
}

void ParticleRenderer::Draw()
{
	if (mIsVisible)
	{
		ParticleCommon::SetBlend(mBlend);
		mParticle->Draw(mPlane.get());
	}
}

// ==================================================
// json
// ==================================================

void ParticleRenderer::Load(const nlohmann::json& json)
{
	RendererComponent::Load(json);
	// Blend
	uint32_t blend = 0;
	JsonHelper::GetUint(json, "Blend", blend);
	mBlend = ParticleCommon::Blend(blend);
	// Particle
	std::string texturePath;
	if (JsonHelper::GetString(json, "Texture", texturePath))
	{
		auto renderer = mOwner->GetScene()->GetRenderer();
		auto texture = renderer->GetTexture(texturePath);
		if (texture)
		{
			SetTexture(texture);
		}
	}
	bool isBillboard;
	JsonHelper::GetBool(json, "Is Billboard", isBillboard);
	mParticle->SetIsBillboard(isBillboard);
	// Emitter
	Emitter emitter = mParticle->GetEmitter();
	JsonHelper::GetFloat(json, "Life", emitter.mLife);
	JsonHelper::GetUint(json, "Amount", emitter.mAmount);
	JsonHelper::GetFloat(json, "Frequency", emitter.mFrequency);
	JsonHelper::GetBool(json, "Is Loop", emitter.mIsLoop);
	JsonHelper::GetFloat(json, "Delay", emitter.mDelay);
	uint32_t shape = 0;
	JsonHelper::GetUint(json, "Shape", shape);
	emitter.mShape = Emitter::Shape(shape);
	JsonHelper::GetVector3(json, "Box Min", emitter.mBoxMin);
	JsonHelper::GetVector3(json, "Box Max", emitter.mBoxMax);
	JsonHelper::GetFloat(json, "Radius", emitter.mSphereRadius);
	// Particle Init
	JsonHelper::GetFloat(json, "Init Life Min", emitter.mInitLife.mMin);
	JsonHelper::GetFloat(json, "Init Life Max", emitter.mInitLife.mMax);
	JsonHelper::GetVector3(json, "Init Vel Min", emitter.mInitVel.mMin);
	JsonHelper::GetVector3(json, "Init Vel Max", emitter.mInitVel.mMax);
	JsonHelper::GetVector3(json, "Init Accel Min", emitter.mInitAccel.mMin);
	JsonHelper::GetVector3(json, "Init Accel Max", emitter.mInitAccel.mMax);
	JsonHelper::GetFloat(json, "Init SScale Min", emitter.mInitSScale.mMin);
	JsonHelper::GetFloat(json, "Init SScale Max", emitter.mInitSScale.mMax);
	JsonHelper::GetFloat(json, "Init EScale Min", emitter.mInitEScale.mMin);
	JsonHelper::GetFloat(json, "Init EScale Max", emitter.mInitEScale.mMax);
	JsonHelper::GetVector3(json, "Init SRot Min", emitter.mInitSRot.mMin);
	JsonHelper::GetVector3(json, "Init SRot Max", emitter.mInitSRot.mMax);
	JsonHelper::GetVector3(json, "Init ERot Min", emitter.mInitERot.mMin);
	JsonHelper::GetVector3(json, "Init ERot Max", emitter.mInitERot.mMax);
	JsonHelper::GetColor(json, "Init SColor Min", emitter.mInitSColor.mMin);
	JsonHelper::GetColor(json, "Init SColor Max", emitter.mInitSColor.mMax);
	JsonHelper::GetColor(json, "Init EColor Min", emitter.mInitEColor.mMin);
	JsonHelper::GetColor(json, "Init EColor Max", emitter.mInitEColor.mMax);
	mParticle->SetEmitter(emitter);
}

void ParticleRenderer::Save(nlohmann::json& json)
{
	RendererComponent::Save(json);
	// Blend
	JsonHelper::SetUint(json, "Blend", uint32_t(mBlend));
	// Particle
	auto mat = mPlane->GetMaterials().find("Effect")->second;
	auto tex = mat->GetTexture();
	if (tex)
	{
		JsonHelper::SetString(json, "Texture", tex->GetFilePath());
	}
	JsonHelper::SetBool(json, "Is Billboard", mParticle->GetIsBillboard());
	// Emitter
	Emitter emitter = mParticle->GetEmitter();
	JsonHelper::SetFloat(json, "Life", emitter.mLife);
	JsonHelper::SetUint(json, "Amount", emitter.mAmount);
	JsonHelper::SetFloat(json, "Frequency", emitter.mFrequency);
	JsonHelper::SetBool(json, "Is Loop", emitter.mIsLoop);
	JsonHelper::SetFloat(json, "Delay", emitter.mDelay);
	JsonHelper::SetUint(json, "Shape", uint32_t(emitter.mShape));
	JsonHelper::SetVector3(json, "Box Min", emitter.mBoxMin);
	JsonHelper::SetVector3(json, "Box Max", emitter.mBoxMax);
	JsonHelper::SetFloat(json, "Radius", emitter.mSphereRadius);
	// Particle Init
	JsonHelper::SetFloat(json, "Init Life Min", emitter.mInitLife.mMin);
	JsonHelper::SetFloat(json, "Init Life Max", emitter.mInitLife.mMax);
	JsonHelper::SetVector3(json, "Init Vel Min", emitter.mInitVel.mMin);
	JsonHelper::SetVector3(json, "Init Vel Max", emitter.mInitVel.mMax);
	JsonHelper::SetVector3(json, "Init Accel Min", emitter.mInitAccel.mMin);
	JsonHelper::SetVector3(json, "Init Accel Max", emitter.mInitAccel.mMax);
	JsonHelper::SetFloat(json, "Init SScale Min", emitter.mInitSScale.mMin);
	JsonHelper::SetFloat(json, "Init SScale Max", emitter.mInitSScale.mMax);
	JsonHelper::SetFloat(json, "Init EScale Min", emitter.mInitEScale.mMin);
	JsonHelper::SetFloat(json, "Init EScale Max", emitter.mInitEScale.mMax);
	JsonHelper::SetVector3(json, "Init SRot Min", emitter.mInitSRot.mMin);
	JsonHelper::SetVector3(json, "Init SRot Max", emitter.mInitSRot.mMax);
	JsonHelper::SetVector3(json, "Init ERot Min", emitter.mInitERot.mMin);
	JsonHelper::SetVector3(json, "Init ERot Max", emitter.mInitERot.mMax);
	JsonHelper::SetColor(json, "Init SColor Min", emitter.mInitSColor.mMin);
	JsonHelper::SetColor(json, "Init SColor Max", emitter.mInitSColor.mMax);
	JsonHelper::SetColor(json, "Init EColor Min", emitter.mInitEColor.mMin);
	JsonHelper::SetColor(json, "Init EColor Max", emitter.mInitEColor.mMax);
}

// ==================================================
// 開発用
// ==================================================

void ParticleRenderer::UpdateForDev()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Particle Renderer"))
	{
		RendererComponent::UpdateForDev();
		// Blend
		Editor::Combo("Blend", mBlend);

		ImGui::Text(std::format("Particle Count: {}", mParticle->GetCount()).c_str());
		// Texture
		ImGui::InputText("Texture Path", &mTexturePath);
		if (ImGui::BeginDragDropTarget())// ドロップ
		{
			if (auto payload = ImGui::AcceptDragDropPayload("TEXTURE_PAYLOAD"))
			{
				auto texture = (Texture**)(payload->Data);
				SetTexture(*texture);
			}
			ImGui::EndDragDropTarget();
		}
		if (ImGui::Button("Load##Texture"))
		{
			auto renderer = mOwner->GetScene()->GetRenderer();
			auto texture = renderer->GetTexture(mTexturePath);
			if (texture)
			{
				SetTexture(texture);
			}
		}
		// Is Billboard
		bool isBillboard = mParticle->GetIsBillboard();
		if (ImGui::Checkbox("Is Billboard", &isBillboard))
		{
			mParticle->SetIsBillboard(isBillboard);
		}

		// Emitter
		Emitter emitter = mParticle->GetEmitter();
		if (ImGui::TreeNode("Emitter"))
		{
			ImGui::DragFloat("Life", &emitter.mLife, 0.01f, emitter.mDelay, MyMath::kInfinity);
			ImGui::Text(std::format("Curr Time: {:6f}", emitter.mCurrTime).c_str());
			ImGui::InputScalar("Amount", ImGuiDataType_U32, &emitter.mAmount);
			ImGui::DragFloat("Frequency", &emitter.mFrequency, 0.01f, 0.01f, MyMath::kInfinity);
			ImGui::Text(std::format("Frequency Time: {:6f}", emitter.mFrequencyTime).c_str());
			ImGui::Checkbox("Is Loop", &emitter.mIsLoop);
			ImGui::DragFloat("Delay", &emitter.mDelay, 0.01f, 0.0f, emitter.mLife);
			Editor::Combo("Shape", emitter.mShape);
			ImGui::DragFloat3("Box.Min", &emitter.mBoxMin.x, 0.01f);
			ImGui::DragFloat3("Box.Max", &emitter.mBoxMax.x, 0.01f);
			ImGui::DragFloat("Sphere.Radius", &emitter.mSphereRadius, 0.01f, 0.0f, MyMath::kInfinity);
			ImGui::TreePop();
		}
		// Particle Init
		if (ImGui::TreeNode("Init"))
		{
			ImGui::DragFloat("Life.Min", &emitter.mInitLife.mMin, 0.01f, 0.0f, emitter.mInitLife.mMax);
			ImGui::DragFloat("Life.Max", &emitter.mInitLife.mMax, 0.01f, emitter.mInitLife.mMin, MyMath::kInfinity);
			ImGui::DragFloat3("Vel.Min", &emitter.mInitVel.mMin.x, 0.01f);
			ImGui::DragFloat3("Vel.Max", &emitter.mInitVel.mMax.x, 0.01f);
			ImGui::DragFloat3("Accel.Min", &emitter.mInitAccel.mMin.x, 0.01f);
			ImGui::DragFloat3("Accel.Max", &emitter.mInitAccel.mMax.x, 0.01f);
			ImGui::DragFloat("SScale.Min", &emitter.mInitSScale.mMin, 0.01f, 0.0f, emitter.mInitSScale.mMax);
			ImGui::DragFloat("SScale.Max", &emitter.mInitSScale.mMax, 0.01f, emitter.mInitSScale.mMin, MyMath::kInfinity);
			ImGui::DragFloat("EScale.Min", &emitter.mInitEScale.mMin, 0.01f, 0.0f, emitter.mInitEScale.mMax);
			ImGui::DragFloat("EScale.Max", &emitter.mInitEScale.mMax, 0.01f, emitter.mInitEScale.mMin, MyMath::kInfinity);
			ImGui::DragFloat3("SRot.Min", &emitter.mInitSRot.mMin.x, 0.01f);
			ImGui::DragFloat3("SRot.Max", &emitter.mInitSRot.mMax.x, 0.01f);
			ImGui::DragFloat3("ERot.Min", &emitter.mInitERot.mMin.x, 0.01f);
			ImGui::DragFloat3("ERot.Max", &emitter.mInitERot.mMax.x, 0.01f);
			ImGui::ColorEdit4("SColor.Min", &emitter.mInitSColor.mMin.r);
			ImGui::ColorEdit4("SColor.Max", &emitter.mInitSColor.mMax.r);
			ImGui::ColorEdit4("EColor.Min", &emitter.mInitEColor.mMin.r);
			ImGui::ColorEdit4("EColor.Max", &emitter.mInitEColor.mMax.r);
			ImGui::TreePop();
		}
		mParticle->SetEmitter(emitter);

		ImGui::TreePop();
	}
}

void ParticleRenderer::RenderForDev(Primitive* prim)
{
	auto emitter = mParticle->GetEmitter();
	//auto position = mOwner->mTransform->mPosition;
	auto position = mOwner->mTransform->GetWorld().GetTranslation();
	switch (emitter.mShape)
	{
	case Emitter::Shape::Point:
		prim->DrawSphere(position, 0.2f, Color::kWhite);
		break;
	case Emitter::Shape::Box:
	{
		auto pos = (emitter.mBoxMin + emitter.mBoxMax) / 2.0f;
		auto size = emitter.mBoxMax - emitter.mBoxMin;
		prim->DrawBox3(position + pos, size, Color::kWhite);
		break;
	}
	case Emitter::Shape::Sphere:
		prim->DrawSphere(position, emitter.mSphereRadius, Color::kWhite);
		break;
	}
}

// ==================================================
// ヘルパー関数
// ==================================================

void ParticleRenderer::SetTexture(Texture* texture)
{
	if (texture)
	{
		auto mat = mPlane->GetMaterials().find("Effect")->second;
		mat->SetTexture(texture);
		mTexturePath = texture->GetFilePath();
	}
}
