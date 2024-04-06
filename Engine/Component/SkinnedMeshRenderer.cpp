#include "SkinnedMeshRenderer.h"
#include "Editor.h"
#include "Graphics/Primitive.h"
#include "Graphics/Renderer.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"
#include "Graphics/Model/SkinCluster.h"

SkinnedMeshRenderer::SkinnedMeshRenderer(Actor* owner)
	: MeshRenderer(owner)
	, mCurrTime(0.0f)
	, mCurrAnim(nullptr)
	, mAnimName()
	, mIsLoop(true)
	, mNextAnim(nullptr)
	, mTransitionTime(0.0f)
{
	mOwner->GetScene()->GetRenderer()->AddSkinnedMesh(this);
}

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
	mOwner->GetScene()->GetRenderer()->RemoveSkinnedMesh(this);
}

void SkinnedMeshRenderer::Update(float deltaTime)
{
	if (mCurrAnim)
	{
		// アニメーションを更新
		mCurrTime += deltaTime;
		if (mNextAnim && mTransitionTime <= kTransitionMax)
		{
			mTransitionTime = MyMath::Min(mTransitionTime + deltaTime, kTransitionMax);
		}
		if (mTransitionTime >= kTransitionMax)
		{
			mTransitionTime = 0.0f;
			mCurrAnim = mNextAnim;
			mNextAnim = nullptr;
		}

		if (mCurrTime >= mCurrAnim->GetDuration() && mIsLoop)
		{
			mCurrTime -= mCurrAnim->GetDuration();
		}

		// メッシュを更新
		for (auto& mesh : mModel->GetMeshes())
		{
			if (mNextAnim)
			{
				if (mCurrTime >= mNextAnim->GetDuration() && mIsLoop)
				{
					mCurrTime -= mNextAnim->GetDuration();
				}
				auto currPoses = mCurrAnim->UpdatePoseAtTime(
					mesh->GetSkeleton(), mNextAnim, mCurrTime, mTransitionTime, mTransitionTime / kTransitionMax);
				mesh->Update(currPoses);
			}
			else
			{
				auto currPoses = mCurrAnim->UpdatePoseAtTime(mesh->GetSkeleton(), mCurrTime);
				mesh->Update(currPoses);
			}
		}
	}
}

void SkinnedMeshRenderer::Draw()
{
	if (mIsVisible && mModel)
	{
		MY_ASSERT(ModelCommon::mCmdList);
		for (auto& mesh : mModel->GetMeshes())
		{
			if (mesh->GetSkeleton())
			{
				// スキンアニメーション
				ModelCommon::SetSkinnedPso(mShaderType);
			}
			else
			{
				ModelCommon::SetPso(mShaderType);
			}
			mOwner->mTransform->Bind(ModelCommon::mCmdList, 0);
			mesh->Draw(ModelCommon::mCmdList, 2, 3);
		}
	}
}

void SkinnedMeshRenderer::PlayAnimation(Animation* anim)
{
	if (!anim)
	{
		return;
	}

	if (anim == mCurrAnim)
	{
		if (mNextAnim)
		{
			mNextAnim = nullptr;
		}
	}

	if (!mCurrAnim)
	{
		mCurrAnim = anim;
	}
	else
	{
		mNextAnim = anim;
		mTransitionTime = 0.0f;
	}
	//mCurrAnim = anim;
	mCurrTime = 0.0f;
}

// ==================================================
// json
// ==================================================

void SkinnedMeshRenderer::Load(const nlohmann::json& json)
{
	MeshRenderer::Load(json);
	// Animation
	JsonHelper::GetFloat(json, "Curr Time", mCurrTime);
	std::string animName;
	JsonHelper::GetString(json, "Curr Anim", animName);
	if (!animName.empty())
	{
		auto renderer = mOwner->GetScene()->GetRenderer();
		mCurrAnim = renderer->GetAnimation(animName);
		if (mCurrAnim)
		{
			mAnimName = mCurrAnim->GetName();
		}
		mCurrTime = 0.0f;
		// メッシュを更新
		for (auto& mesh : mModel->GetMeshes())
		{
			//mesh->Update(mCurrAnim, mCurrTime);
			auto currPoses = mCurrAnim->UpdatePoseAtTime(mesh->GetSkeleton(), mCurrTime);
			mesh->Update(currPoses);
		}
	}
}

void SkinnedMeshRenderer::Save(nlohmann::json& json)
{
	MeshRenderer::Save(json);
	// Animation
	JsonHelper::GetFloat(json, "Curr Time", mCurrTime);
	if (mCurrAnim)
	{
		JsonHelper::SetString(json, "Curr Anim", mCurrAnim->GetName());
	}
}

// ==================================================
// 開発用
// ==================================================

void SkinnedMeshRenderer::UpdateForDev()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Skinned Mesh Renderer"))
	{
		// Model
		UpdateModel();
		// Curr Time
		float duration = 0.0f;
		if (mCurrAnim)
		{
			duration = mCurrAnim->GetDuration();
		}
		ImGui::SliderFloat("Curr Time", &mCurrTime, 0.0f, duration);
		// Curr Anim
		ImGui::InputText("Anim Name", &mAnimName);
		if (ImGui::BeginDragDropTarget())// ドロップ
		{
			if (auto payload = ImGui::AcceptDragDropPayload("ANIM_PAYLOAD"))
			{
				mCurrAnim = *(Animation**)(payload->Data);
				mAnimName = mCurrAnim->GetName();
			}
			ImGui::EndDragDropTarget();
		}
		if (ImGui::Button("Load##Anim"))
		{
			auto renderer = mOwner->GetScene()->GetRenderer();
			auto anim = renderer->GetAnimation(mAnimName);
			if (anim)
			{
				mCurrAnim = anim;
			}
		}
		ImGui::TreePop();
	}
}

void SkinnedMeshRenderer::RenderForDev(Primitive* prim)
{
	if (mModel)
	{
		if (mModel->GetMeshes()[0]->GetSkeleton() && mCurrAnim)
		{
			auto& joints = mModel->GetMeshes()[0]->GetSkeleton()->GetJoints();
			std::vector<Vector3> positions(joints.size());
			auto currPoses = mModel->GetMeshes()[0]->GetCurrPoses();
			if (currPoses.size() > 0)
			{
				for (uint32_t i = 0; i < joints.size(); ++i)
				{
					positions[i] = (currPoses[i] * mOwner->mTransform->GetWorld()).GetTranslate();
					prim->DrawSphere(positions[i], 0.2f, Color::kWhite);// radius = 0.2f
					auto parent = joints[i].mParent;
					if (parent)
					{
						prim->DrawLine3(positions[i], positions[*parent], Color::kWhite);
					}
				}
			}
		}
	}
}
