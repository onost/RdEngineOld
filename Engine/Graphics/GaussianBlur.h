#pragma once
//#include "Core/ConstantBuffer.h"
#include "Core/PipelineState.h"
#include "Core/RenderTarget.h"
#include "Core/RootSignature.h"
#include "Core/StructuredBuffer.h"
#include "Sprite/Sprite.h"

class Texture;
class Renderer;

// ガウシアンブラー
class GaussianBlur
{
public:
	void Initialize(Texture* texture, Renderer* renderer);
	// ブラーを実行
	void Execute(ID3D12GraphicsCommandList* cmdList, float power = 20.0f);

	// ブラーをかけたテクスチャ
	Texture* GetTexture()
	{
		return mVBlurRt.GetRenderTarget().get();
	}

private:
	// 元テクスチャ
	Texture* mTexture;
	// パイプライン
	std::unique_ptr<RootSignature> mBlurRs;
	PipelineState mHBlurPso;// 横ブラー
	PipelineState mVBlurPso;// 縦ブラー

	RenderTarget mHBlurRt;
	RenderTarget mVBlurRt;
	Sprite mHBlurSprite;
	Sprite mVBlurSprite;

	// 重み
	std::unique_ptr<ConstantBuffer> mCBuff;
	float mWeights[8];
};
