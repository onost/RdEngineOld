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
	//, mPoses()
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
		if (mCurrTime >= mCurrAnim->mDuration)
		{
			mCurrTime -= mCurrAnim->mDuration;
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

void SkinnedMeshRenderer::PlayAnimation(Animation* anim)
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
	/*auto invBindPoses = mSkeleton->GetInvBindPoses();
	auto currPoses = mCurrAnim->GetPoseAtTime(mSkeleton, mCurrTime);*/
	if (mSkeleton && mCurrAnim)
	{
		mSkeleton->ApplyAnimation(*mCurrAnim, mCurrTime);
		mSkeleton->Update();
		auto data = static_cast<Matrix4*>(mCBuff->GetData());
		for (uint32_t i = 0; i < mSkeleton->mJoints.size(); ++i)
		{
			data[i] = Matrix4::kIdentity;// invBindPoses[i] * currPoses[i];
		}
		// 開発用
		//mPoses = currPoses;
	}
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
			mSkelName = mSkeleton->mName;
			//mPoses.resize(mSkeleton->mJoints.size());
		}
	}
	// Animation
	JsonHelper::GetFloat(json, "Curr Time", mCurrTime);
	std::string animName;
	if (JsonHelper::GetString(json, "Curr Anim", animName))
	{
		auto renderer = mOwner->GetScene()->GetRenderer();
		mCurrAnim = renderer->GetAnimation(animName);
		if (mCurrAnim)
		{
			mAnimName = mCurrAnim->mName;
		}
	}
}

void SkinnedMeshRenderer::Save(nlohmann::json& json)
{
	MeshRenderer::Save(json);
	// Skeleton
	if (mSkeleton)
	{
		JsonHelper::SetString(json, "Skeleton", mSkeleton->mName);
	}
	// Animation
	JsonHelper::GetFloat(json, "Curr Time", mCurrTime);
	if (mCurrAnim)
	{
		JsonHelper::SetString(json, "Curr Anim", mCurrAnim->mName);
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
				mSkelName = mSkeleton->mName;
				//mPoses.resize(mSkeleton->mJoints.size());
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
				//mPoses.resize(mSkeleton->mJoints.size());
			}
		}
		// Curr Time
		float duration = 0.0f;
		if (mCurrAnim)
		{
			duration = mCurrAnim->mDuration;
		}
		ImGui::SliderFloat("Curr Time", &mCurrTime, 0.0f, duration);
		// Curr Anim
		ImGui::InputText("Anim Name", &mAnimName);
		if (ImGui::BeginDragDropTarget())// ドロップ
		{
			if (auto payload = ImGui::AcceptDragDropPayload("ANIM_PAYLOAD"))
			{
				mCurrAnim = *(Animation**)(payload->Data);
				mAnimName = mCurrAnim->mName;
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
	if (mSkeleton && mCurrAnim)
	{
		auto& joints = mSkeleton->mJoints;
		std::vector<Vector3> positions(joints.size());
		for (uint32_t i = 0; i < joints.size(); ++i)
		{
			positions[i] = (joints[i].mSkeletonSpaceMat * mOwner->mTransform->GetWorld()).GetTranslation();
			prim->DrawSphere(positions[i], 0.2f, Color::kWhite);// radius = 0.2f
			auto parent = joints[i].mParent;
			if (parent)
			{
				prim->DrawLine3(positions[i], positions[*parent], Color::kWhite);
			}
		}

		Console::Log(std::format("{},{},{}", positions[30].x, positions[30].y, positions[30].z));
	}
}
