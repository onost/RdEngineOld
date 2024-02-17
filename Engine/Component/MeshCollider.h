#pragma once
#include "Collider.h"
#include "Collision/Collision.h"

class Model;

// メッシュコライダー
class MeshCollider : public Collider
{
public:
	MeshCollider(Actor* owner);
	~MeshCollider();
	void OnUpdateWorld() override;
	// レイキャスト
	bool Raycast(const Ray& ray, float& outT, Vector3& normal);

	bool Dispatch(Collider* /*other*/, CollisionInfo& /*info*/) override { return false; }
	bool IsCollision(SphereCollider* /*other*/, CollisionInfo& /*info*/) override { return false; }
	bool IsCollision(BoxCollider* /*other*/, CollisionInfo& /*info*/) override { return false; }
	//bool IsCollision(MeshCollider* other, CollisionInfo& info) override;

	// ==================================================
	// json
	// ==================================================
	void Load(const nlohmann::json& json) override;
	void Save(nlohmann::json& json) override;

	// ==================================================
	// 開発用
	// ==================================================
	void UpdateForDev() override;

	// ==================================================
	// ヘルパー関数
	// ==================================================
	Type GetType() const override { return Type::MeshCollider; }
	void SetModel(Model* model);// cpp

private:
	// 三角形を作成
	void CreateTriangles();

private:
	std::vector<Triangle> mTriangles;// 三角形
	Matrix4 mInverseWorld;// 逆ワールド行列
	Model* mModel;
	std::string mModelName;// 開発用
};
