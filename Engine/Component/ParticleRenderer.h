#pragma once
#include "Graphics/Model/Model.h"
#include "Graphics/Particle/ParticleCommon.h"
#include "Graphics/Particle/ParticleManager.h"
#include "RendererComponent.h"

// パーティクルレンダラー
class ParticleRenderer : public RendererComponent
{
public:
	ParticleRenderer(Actor* owner);
	~ParticleRenderer();
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
	Type GetType() const override { return Type::ParticleRenderer; }
	ParticleCommon::Blend GetBlend() const { return mBlend; }
	ParticleManager* GetParticle() const { return mParticle.get(); }
	void SetBlend(ParticleCommon::Blend blend) { mBlend = blend; }
	void SetTexture(Texture* texture);// cpp

protected:
	ParticleCommon::Blend mBlend;
	std::unique_ptr<ParticleManager> mParticle;
	std::unique_ptr<Model> mPlane;
	// 開発用
	std::string mTexturePath;
};
