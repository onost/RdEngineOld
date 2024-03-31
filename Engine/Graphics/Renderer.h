#pragma once
#include "Actor/DebugCamera.h"
#include "Core/RenderTarget.h"
#include "Core/ResourceBank.h"
#include "Core/Shader.h"
#include "GaussianBlur.h"
#include "LightManager.h"
#include "Model/Animation.h"
#include "Model/Material.h"
#include "Model/Model.h"
#include "Model/Skeleton.h"
#include "Primitive.h"
#include "Texture.h"

class CameraComponent;
class MeshParticleRenderer;
class MeshRenderer;
class ParticleRenderer;
class SkinnedMeshRenderer;
class SpriteRenderer;

// レンダラー
class Renderer
{
public:
	void Initialize();
	//void Terminate();

	void LoadFile(const nlohmann::json& json);
	void SaveFile(nlohmann::json& json);

	// シーン描画前
	void PreRendering(ID3D12GraphicsCommandList* cmdList);
	// シーン描画後
	void PostRendering(ID3D12GraphicsCommandList* cmdList);

	// シーン描画
	void RenderScene(ID3D12GraphicsCommandList* cmdList);
	// 最終結果を描画
	void RenderFinal(ID3D12GraphicsCommandList* cmdList);

	void Render(ID3D12GraphicsCommandList* cmdList);

	// スプライトコンポーネントをソート
	void SortSprites(SpriteRenderer* sprite);

	// ==================================================
	// アセット
	// ==================================================
	Texture* GetTexture(const std::string& filePath);
	Model* GetModel(const std::string& filePath);
	void AddAnimation(const std::string& filePath, std::shared_ptr<Animation> animation);
	Animation* GetAnimation(const std::string& filePath);
	Shader* GetVs(const std::string& filePath);
	Shader* GetGs(const std::string& filePath);
	Shader* GetPs(const std::string& filePath);
	void AddSkeleton(const std::string& filePath, std::shared_ptr<Skeleton> skeleton);
	Skeleton* GetSkeleton(const std::string& filePath);

	// ==================================================
	// レンダラーコンポーネント
	// ==================================================
	// Add
	void AddSprite(SpriteRenderer* sprite);
	void AddMesh(MeshRenderer* mesh);
	void AddSkinnedMesh(SkinnedMeshRenderer* skinnedMesh);
	void AddParticle(ParticleRenderer* particle);
	void AddMeshParticle(MeshParticleRenderer* meshParticle);
	// Remove
	void RemoveSprite(SpriteRenderer* sprite);
	void RemoveMesh(MeshRenderer* mesh);
	void RemoveSkinnedMesh(SkinnedMeshRenderer* skinnedMesh);
	void RemoveParticle(ParticleRenderer* particle);
	void RemoveMeshParticle(MeshParticleRenderer* meshParticle);

	// ==================================================
	// 開発用
	// ==================================================
	void UpdateForDev();
	void RenderDebug(ID3D12GraphicsCommandList* cmdList);

	// ==================================================
	// ヘルパー関数
	// ==================================================
	Camera* GetCurrCamera() const { return mCurrCamera; }
	//DebugCamera* GetDebugCamera() const { return mDebugCamera.get(); }
	//bool GetIsDebugCamera()const { return mIsDebugCamera; }
	Primitive* GetPrimitive() const { return mPrimitive.get(); }
	LightManager* GetLightManager() const { return mLightManager.get(); }
	void SetGameCamera(CameraComponent* camera);// cpp
	//void SetIsDebugCamera(bool isDebugCamera) { mIsDebugCamera = isDebugCamera; }

private:
	void Load();

private:
	// アセット
	ResourceBank<Texture> mTextures;// テクスチャ
	ResourceBank<Model> mModels;// モデル
	ResourceBank<Material> mMaterials;// マテリアル
	ResourceBank<Skeleton> mSkeletons;// スケルトン
	ResourceBank<Animation> mAnimations;// アニメーション
	ResourceBank<Shader> mShaders;// シェーダ
	// モデル用関数テーブル
	/*typedef Model* (Renderer::* ModelFunc)(const std::string&);
	std::unordered_map<std::string, ModelFunc> mModelFuncs;*/

	// レンダラーコンポーネント
	std::vector<SpriteRenderer*> mBackground;// 背景スプライト
	std::vector<SpriteRenderer*> mForeground;// 前景スプライト
	std::vector<MeshRenderer*> mMeshes;// メッシュ
	std::vector<SkinnedMeshRenderer*> mSkinnedMeshes;// スキンメッシュ
	std::vector<ParticleRenderer*> mParticles;// パーティクル
	std::vector<MeshParticleRenderer*> mMeshParticles;// メッシュパーティクル

	// カメラ
	Camera* mCurrCamera;
	CameraComponent* mGameCamera;
	//std::unique_ptr<DebugCamera> mDebugCamera;// デバッグカメラ
	//bool mIsDebugCamera = true;
	// ライト管理
	std::unique_ptr<LightManager> mLightManager;
	// プリミティブ
	std::unique_ptr<Primitive> mPrimitive;

	// レンダーターゲット
	std::unique_ptr<RenderTarget> mMainRt;
	std::unique_ptr<RenderTarget> mFinalRt;
	std::unique_ptr<Sprite> mFinalSprite;
	// ガウシアンブラー
	std::unique_ptr<GaussianBlur> mGaussianBlur;
	bool mIsGaussianBlur = false;
	float mGaussianPower = 20.0f;

	std::string mAnimName;
};
