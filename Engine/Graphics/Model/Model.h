#pragma once
#include "Material.h"
#include "Mesh.h"
#include "ModelCommon.h"
#include <string>
#include <unordered_map>
#include <vector>

class SkeletonOld;
class Transform;

// モデル
class Model
{
	friend class ObjLoader;
	friend class ModelLoader;
public:
	~Model();
	void Create(const std::string& name);
	void Draw(Transform* transform,
		ModelCommon::Type type = ModelCommon::Type::Default);
	// スキンアニメーション用
	void Draw(Transform* transform, SkeletonOld* skeleton,
		ModelCommon::Type type = ModelCommon::Type::Default);

	//Model* Copy();

	const std::string& GetName() const { return mName; }
	//const std::vector<std::unique_ptr<Mesh>>& GetMeshes() { return mMeshes; }
	//const std::unordered_map<std::string, std::unique_ptr<Material>>& GetMaterials() { return mMaterials; }
	const std::vector<Mesh*>& GetMeshes() const { return mMeshes; }
	const std::unordered_map<std::string, Material*>& GetMaterials() const { return mMaterials; }

private:
	std::string mName;
	//std::vector<std::unique_ptr<Mesh>> mMeshes;
	//std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
	std::vector<Mesh*> mMeshes;
	std::unordered_map<std::string, Material*> mMaterials;
};
