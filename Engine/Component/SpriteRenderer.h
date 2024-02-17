#pragma once
#include "RendererComponent.h"
#include "Graphics/Sprite/Sprite.h"
#include "Graphics/Sprite/SpriteCommon.h"

// スプライトレンダラー
class SpriteRenderer : public RendererComponent
{
public:
	SpriteRenderer(Actor* owner, uint32_t drawOrder = 200);
	~SpriteRenderer();

	void Draw() override;

	// ==================================================
	// json
	// ==================================================
	void Load(const nlohmann::json& json) override;
	void Save(nlohmann::json& json) override;

	// ==================================================
	// 開発用
	// ==================================================
	void UpdateForDev() override;
	void RenderForDev(Primitive*) override;

	// ==================================================
	// ヘルパー関数
	// ==================================================
	Type GetType() const override { return Type::SpriteRenderer; }
	uint32_t GetDrawOrder() const { return mDrawOrder; }
	SpriteCommon::Blend GetBlend() const { return mBlend; }
	Sprite* GetSprite() const { return mSprite.get(); }
	void SetDrawOrder(uint32_t drawOrder);// cpp
	void SetBlend(SpriteCommon::Blend blend) { mBlend = blend; }
	// Sprite
	Texture* GetTexture() const { return mSprite->GetTexture(); }
	const Vector2& GetPivot() const { return mSprite->GetPivot(); }
	bool GetIsFlipX() const { return mSprite->GetIsFlipX(); }
	bool GetIsFlipY() const { return mSprite->GetIsFlipY(); }
	const Vector4& GetRect() const { return mSprite->GetRect(); }
	const Color& GetColor() const { return mSprite->GetColor(); }
	void SetTexture(Texture* texture) { mSprite->SetTexture(texture); }
	void SetPivot(const Vector2& pivot) { mSprite->SetPivot(pivot); }
	void SetIsFlipX(bool isFlipX) { mSprite->SetIsFlipX(isFlipX); }
	void SetIsFlipY(bool isFlipY) { mSprite->SetIsFlipY(isFlipY); }
	void SetRect(const Vector4& rect) { mSprite->SetRect(rect); }
	void SetColor(const Color& color) { mSprite->SetColor(color); }

private:
	uint32_t mDrawOrder;// 描画順
	SpriteCommon::Blend mBlend;
	std::unique_ptr<Sprite> mSprite;
	std::string mTexturePath;// 開発用
};
