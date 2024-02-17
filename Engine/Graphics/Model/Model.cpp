#include "Model.h"
#include "Component/Transform.h"
#include "Skeleton.h"

Model::~Model()
{
	// メッシュ
	for (auto& mesh : mMeshes)
	{
		delete mesh;
	}
	// マテリアル
	for (auto& material : mMaterials)
	{
		delete material.second;
	}
}

void Model::Create(const std::string& name)
{
	mName = name;
	// メッシュ
	for (auto& mesh : mMeshes)
	{
		mesh->Create(/*ModelCommon::mRenderer*/);
	}
	// マテリアル
	for (auto& material : mMaterials)
	{
		material.second->Create(ModelCommon::mRenderer);
	}
}

/*
void Model::Draw(Transform* transform, ModelCommon::Type type)
{
	MyAssert(ModelCommon::mCmdList);
	MyAssert(transform);

	ModelCommon::SetPso(type);
	for (auto& mesh : mMeshes)
	{
		transform->Bind(ModelCommon::mCmdList, 0);
		mesh->Draw(ModelCommon::mCmdList, 2, 3);
	}
}

// スキンアニメーション用
void Model::Draw(Transform* transform, Skeleton* skeleton, ModelCommon::Type type)
{
	MyAssert(ModelCommon::mCmdList);
	MyAssert(transform);

	for (auto& mesh : mMeshes)
	{
		if (mesh->GetIsSkinned() && skeleton)
		{
			// スキンアニメーション
			ModelCommon::SetSkinnedPso(type);
			skeleton->Bind(ModelCommon::mCmdList, 5);
		}
		else
		{
			ModelCommon::SetPso(type);
		}
		transform->Bind(ModelCommon::mCmdList, 0);
		mesh->Draw(ModelCommon::mCmdList, 2, 3);
	}
}
*/

/*
Model* Model::Copy()
{
	Model* model = new Model();
	model->mName = mName;
	// メッシュ
	for (auto& mesh : mMeshes)
	{
		model->mMeshes.emplace_back(mesh);
	}
	// マテリアル
	for (auto& material : mMaterials)
	{
		model->mMaterials.emplace(material.first, material.second);
	}
	//model->mMeshes = mMeshes;
	//model->mMaterials = mMaterials;
	return model;
}
*/
