#include "SkinnedMeshRenderer.h"
#include "Editor.h"
#include "Graphics/Primitive.h"
#include "Graphics/Renderer.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"

SkinnedMeshRenderer::SkinnedMeshRenderer(Actor* owner)
	: MeshRenderer(owner)
	, mSkeleton(nullptr)
	, mCurrTime(0.0f)
	, mCurrAnim(nullptr)
	, mCBuff(nullptr)
	//, mCurrPoses()
	, mSkelName()
	, mAnimName()
	, mPoses()
{
	mOwner->GetScene()->GetRenderer()->AddSkinnedMesh(this);
	mCBuff = std::make_unique<ConstantBuffer>();
	mCBuff->Create(sizeof(Matrix4) * 128);// 128: Max Bone
}

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
	mOwner->GetScene()->GetRenderer()->RemoveSkinnedMesh(this);
}

void SkinnedMeshRenderer::Update(float deltaTime)
{
	if (mSkeleton && mCurrAnim)
	{
		// アニメーションを更新
		mCurrTime += deltaTime;
		if (mCurrTime >= mCurrAnim->GetDuration())
		{
			mCurrTime -= mCurrAnim->GetDuration();
		}
		ComputePose();
	}
}

void SkinnedMeshRenderer::Draw()
{
	if (mIsVisible && mModel)
	{
		MyAssert(ModelCommon::mCmdList);
		for (auto& mesh : mModel->GetMeshes())
		{
			if (mesh->GetIsSkinned() && mSkeleton)
			{
				// スキンアニメーション
				ModelCommon::SetSkinnedPso(mShaderType);
				mCBuff->Bind(ModelCommon::mCmdList, 5);
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

void SkinnedMeshRenderer::PlayAnimation(AnimationOld* anim)
{
	if (!anim)
	{
		return;
	}
	mCurrAnim = anim;
	mCurrTime = 0.0f;
	ComputePose();
}

void SkinnedMeshRenderer::ComputePose()
{
	auto invBindPoses = mSkeleton->GetInvBindPoses();
	auto currPoses = mCurrAnim->GetPoseAtTime(mSkeleton, mCurrTime);
	auto data = static_cast<Matrix4**>(mCBuff->GetData());
	for (uint32_t i = 0; i < mSkeleton->GetBoneCount(); ++i)
	{
		*data[i] = invBindPoses[i] * currPoses[i];
	}
	// 開発用
	mPoses = currPoses;
}

// ==================================================
// json
// ==================================================

void SkinnedMeshRenderer::Load(const nlohmann::json& json)
{
	MeshRenderer::Load(json);
	// Skeleton
	std::string skeletonName;
	if (JsonHelper::GetString(json, "Skeleton", skeletonName))
	{
		auto renderer = mOwner->GetScene()->GetRenderer();
		mSkeleton = renderer->GetSkeleton(skeletonName);
		if (mSkeleton)
		{
			mSkelName = mSkeleton->GetName();
			mPoses.resize(mSkeleton->GetBoneCount());
		}
	}
	// Animation
	JsonHelper::GetFloat(json, "Curr Time", mCurrTime);
	std::string animName;
	if (JsonHelper::GetString(json, "Curr Anim", animName))
	{
		auto renderer = mOwner->GetScene()->GetRenderer();
		mCurrAnim = renderer->GetAnimationOld(animName);
		if (mCurrAnim)
		{
			mAnimName = mCurrAnim->GetName();
		}
	}
}

void SkinnedMeshRenderer::Save(nlohmann::json& json)
{
	MeshRenderer::Save(json);
	// Skeleton
	if (mSkeleton)
	{
		JsonHelper::SetString(json, "Skeleton", mSkeleton->GetName());
	}
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
		// Skeleton
		ImGui::InputText("Skel Name", &mSkelName);
		if (ImGui::BeginDragDropTarget())// ドロップ
		{
			if (auto payload = ImGui::AcceptDragDropPayload("SKEL_PAYLOAD"))
			{
				mSkeleton = *(Skeleton**)(payload->Data);
				mSkelName = mSkeleton->GetName();
				mPoses.resize(mSkeleton->GetBoneCount());
			}
			ImGui::EndDragDropTarget();
		}
		if (ImGui::Button("Load##Skel"))
		{
			auto renderer = mOwner->GetScene()->GetRenderer();
			auto skel = renderer->GetSkeleton(mSkelName);
			if (skel)
			{
				mSkeleton = skel;
				mPoses.resize(mSkeleton->GetBoneCount());
			}
		}
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
				mCurrAnim = *(AnimationOld**)(payload->Data);
				mAnimName = mCurrAnim->GetName();
			}
			ImGui::EndDragDropTarget();
		}
		if (ImGui::Button("Load##Anim"))
		{
			auto renderer = mOwner->GetScene()->GetRenderer();
			auto anim = renderer->GetAnimationOld(mAnimName);
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
	if (mSkeleton && mCurrAnim)
	{
		auto& bones = mSkeleton->GetBones();
		std::vector<Vector3> positions(bones.size());
		for (uint32_t i = 0; i < bones.size(); ++i)
		{
			positions[i] = mPoses[i].GetTranslation() * mOwner->mTransform->GetWorld();
			prim->DrawSphere(positions[i], 0.2f, Color::kWhite);// radius = 0.2f
			int32_t parent = bones[i].mParent;
			if (parent != -1)
			{
				prim->DrawLine3(positions[i], positions[parent], Color::kWhite);
			}
		}
	}
}
