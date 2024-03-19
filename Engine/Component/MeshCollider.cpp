#include "MeshCollider.h"
#include "Actor/Actor.h"
#include "Editor.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Renderer.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"

MeshCollider::MeshCollider(Actor* owner)
	: Collider(owner)
	, mTriangles()
	, mInverseWorld(Matrix4::kIdentity)
	, mModel(nullptr)
	, mModelName()
{
	SetAttribute(CollisionAttr::Terrain);
}

MeshCollider::~MeshCollider()
{

}

void MeshCollider::OnUpdateWorld()
{
	mInverseWorld = Inverse(mOwner->mTransform->GetWorld());
}

// レイキャスト
bool MeshCollider::Raycast(const Ray& ray, float& outT, Vector3& normal)
{
	Ray localRay = Ray(ray.mStart * mInverseWorld, ray.mEnd * mInverseWorld);
	bool isHit = false;
	float minT = MyMath::kInfinity;
	Triangle* hitTri = nullptr;
	auto it = mTriangles.begin();
	for (; it != mTriangles.end(); ++it)
	{
		float t;
		if (Intersect(localRay, *it, t))
		{
			if (t < minT)
			{
				isHit = true;
				minT = t;
				hitTri = &(*it);
			}
		}
	}
	if (isHit)
	{
		normal = hitTri->GetNormal();
		normal *= Transpose(mInverseWorld);
		normal.Normalize();
		outT = minT;
		return true;
	}
	return false;
}

// 三角形を作成
void MeshCollider::CreateTriangles()
{
	mTriangles.clear();

	auto& meshes = mModel->GetMeshes();
	auto it = meshes.begin();
	uint32_t idx = 0;
	for (; it != meshes.end(); ++it)
	{
		auto& mesh = *it;
		auto& vertices = mesh->GetVertices();
		for (auto v : vertices)
		{
			v.mPos *= mesh->GetLocal();
		}
		auto& indices = mesh->GetIndices();
		uint32_t size = uint32_t(indices.size() / 3);
		mTriangles.resize(mTriangles.size() + size);
		for (uint32_t i = 0; i < size; ++i)
		{
			mTriangles[idx + i] = Triangle(
				vertices[indices[i * 3 + 0]].mPos,
				vertices[indices[i * 3 + 1]].mPos,
				vertices[indices[i * 3 + 2]].mPos);
		}
		idx += uint32_t(size);
	}
}

// ==================================================
// json
// ==================================================

void MeshCollider::Load(const nlohmann::json& json)
{
	Collider::Load(json);
	// Model
	std::string modelName;
	if (JsonHelper::GetString(json, "Model", modelName))
	{
		auto renderer = mOwner->GetScene()->GetRenderer();
		auto model = renderer->GetModel(modelName);
		SetModel(model);
	}
}

void MeshCollider::Save(nlohmann::json& json)
{
	Collider::Save(json);
	if (mModel)
	{
		JsonHelper::SetString(json, "Model", mModel->GetName());
	}
}

// ==================================================
// 開発用
// ==================================================

void MeshCollider::UpdateForDev()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Mesh Collider"))
	{
		Collider::UpdateForDev();
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
		ImGui::TreePop();
	}
}

// ==================================================
// ヘルパー関数
// ==================================================

void MeshCollider::SetModel(Model* model)
{
	if (model)
	{
		mModel = model;
		mModelName = mModel->GetName();
		CreateTriangles();
	}
}
