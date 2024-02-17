#pragma once
#include "Graphics/Model/ModelCommon.h"
#include "Graphics/Particle/ParticleCommon.h"
#include "Graphics/Particle/ParticleManager.h"
#include "RendererComponent.h"

class Model;

// パーティクルレンダラー
class MeshParticleRenderer : public RendererComponent
{
public:
	MeshParticleRenderer(Actor* owner);
	~MeshParticleRenderer();
	void Update(float deltaTime) override;
	void Draw() override;

	void Play() { mParticle->SetIsPlaying(true); }
	void Stop() { mParticle->SetIsPlaying(false); }
	void Reset() { mParticle->Reset(); }

	// ==================================================
	// json
	// ==================================================
	void Load(const nlohmann::json& json) override;
	void Save(nlohmann::json& json) override;

	// ==================================================
	// 開発用
	// ==================================================
	void UpdateForDev() override;
	void RenderForDev(Primitive* prim) override;

	// ==================================================
	// ヘルパー関数
	// ==================================================
	Type GetType() const override { return Type::MeshParticleRenderer; }
	ParticleManager* GetParticle() const { return mParticle.get(); }
	void SetModel(Model* model);// cpp

protected:
	std::unique_ptr<ParticleManager> mParticle;
	Model* mModel;
	ModelCommon::Type mShaderType;// シェーダタイプ
	// 開発用
	std::string mModelName;
};
