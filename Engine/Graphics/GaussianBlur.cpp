#include "GaussianBlur.h"
#include "Core/GraphicsCommon.h"
#include "Sprite/SpriteCommon.h"
#include "Renderer.h"

void GaussianBlur::Initialize(Texture* texture, Renderer* renderer)
{
	MY_ASSERT(texture);
	MY_ASSERT(renderer);
	mTexture = texture;

	// ルートシグネチャ
	//mBlurRs.Initialize(3, 1);
	mBlurRs = std::make_unique<RootSignature>(3, 1);
	mBlurRs->RootParameters(0).InitConstant(0);
	mBlurRs->RootParameters(1).InitDescriptorTable(1);
	mBlurRs->RootParameters(1).InitDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	mBlurRs->RootParameters(2).InitConstant(1);
	mBlurRs->Samplers(0) = GraphicsCommon::gSamplerLinearClamp;
	mBlurRs->Create();

	// シェーダ
	Shader* hBlurVs = renderer->GetVs("Assets/Shader/GaussianBlur/HorizontalBlurVs.hlsl");
	Shader* vBlurVs = renderer->GetVs("Assets/Shader/GaussianBlur/VerticalBlurVs.hlsl");
	Shader* ps = renderer->GetPs("Assets/Shader/GaussianBlur/BlurPs.hlsl");
	// パイプラインステート
	// 横ブラー
	mHBlurPso.SetRootSignature(mBlurRs.get());
	mHBlurPso.SetVertexShader(hBlurVs);
	mHBlurPso.SetPixelShader(ps);
	mHBlurPso.SetBlendState(GraphicsCommon::gBlendNormal);
	mHBlurPso.SetRasterizerState(GraphicsCommon::gRasterizerCullModeNone);
	mHBlurPso.SetDepthStencilState(GraphicsCommon::gDepthDisable);
	D3D12_INPUT_ELEMENT_DESC inputLayouts[2] = {};
	inputLayouts[0].SemanticName = "POSITION";
	inputLayouts[0].SemanticIndex = 0;
	inputLayouts[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputLayouts[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayouts[1].SemanticName = "TEXCOORD";
	inputLayouts[1].SemanticIndex = 0;
	inputLayouts[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputLayouts[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	mHBlurPso.SetInputLayout(_countof(inputLayouts), inputLayouts);
	mHBlurPso.Create();
	// 縦ブラー
	mVBlurPso = mHBlurPso;
	mVBlurPso.SetVertexShader(vBlurVs);
	mVBlurPso.Create();

	// 縮小バッファ
	mHBlurRt.Create(Window::kWidth / 2, Window::kHeight);
	mHBlurSprite.Create(mTexture);
	mVBlurRt.Create(Window::kWidth / 2, Window::kHeight / 2);
	mVBlurSprite.Create(mHBlurRt.GetRenderTarget().get());

	mCBuff = std::make_unique<ConstantBuffer>();
	mCBuff->Create(sizeof(mWeights));
}

void GaussianBlur::Execute(ID3D12GraphicsCommandList* cmdList, float power)
{
	// 重みを計算
	float total = 0;
	for (uint32_t i = 0; i < 8; ++i)
	{
		mWeights[i] = expf(-0.5f * (float)(i * i) / power);
		total += 2.0f * mWeights[i];
	}
	for (uint32_t i = 0; i < 8; ++i)
	{
		mWeights[i] /= total;
	}
	mCBuff->Copy(mWeights);

	mBlurRs->Bind(cmdList);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// 横ブラー
	mHBlurRt.PreRender(cmdList);
	D3D12_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = FLOAT(Window::kWidth) / 2.0f;
	viewport.Height = FLOAT(Window::kHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	cmdList->RSSetViewports(1, &viewport);
	mHBlurPso.Bind(cmdList);
	mCBuff->Bind(cmdList, 2);
	mHBlurSprite.Draw(Vector2(float(Window::kWidth), float(Window::kHeight)));
	mHBlurRt.PostRender();
	// 縦ブラー
	mVBlurRt.PreRender(cmdList);
	viewport.Height = FLOAT(Window::kHeight) / 2.0f;
	cmdList->RSSetViewports(1, &viewport);
	mVBlurPso.Bind(cmdList);
	mCBuff->Bind(cmdList, 2);
	mVBlurSprite.Draw(Vector2(float(Window::kWidth), float(Window::kHeight)));
	mVBlurRt.PostRender();
}
