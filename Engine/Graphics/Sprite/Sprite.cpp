#include "Sprite.h"
#include "Component/Transform.h"
#include "Graphics/Texture.h"
#include "SpriteCommon.h"

Sprite::Sprite()
	: mVBuff(nullptr)
	, mCBuff(nullptr)
	, mTexture(nullptr)
	, mPivot(Vector2(0.5f, 0.5f))
	, mIsFlipX(false)
	, mIsFlipY(false)
	, mRect(Vector4::kZero)
	, mColor(Color::kWhite)
{

}

void Sprite::Create(Texture* texture)
{
	SetTexture(texture);
	// 頂点バッファ
	mVBuff = std::make_unique<VertexBuffer>();
	mVBuff->Create(sizeof(SpriteCommon::Vertex) * 4, sizeof(SpriteCommon::Vertex));
	// 定数バッファ
	mCBuff = std::make_unique<ConstantBuffer>();
	mCBuff->Create(sizeof(SpriteCommon::Constant));
}

void Sprite::Draw(Transform* transform)
{
	MY_ASSERT(transform);
	MY_ASSERT(SpriteCommon::mCmdList);
	if (mTexture)
	{
		// 頂点
		SpriteCommon::Vertex v[4] = {};
		enum { LT, RT, LB, RB };
		{
			float w = float(mRect.z - mRect.x);
			float h = float(mRect.w - mRect.y);
			float l = (0.0f - mPivot.x) * w;
			float t = (0.0f - mPivot.y) * h;
			float r = (1.0f - mPivot.x) * w;
			float b = (1.0f - mPivot.y) * h;
			if (mIsFlipX)
			{
				l = -l;
				r = -r;
			}
			if (mIsFlipY)
			{
				t = -t;
				b = -b;
			}
			v[LT].mPos = Vector3(l, t, 0.0f);
			v[RT].mPos = Vector3(r, t, 0.0f);
			v[LB].mPos = Vector3(l, b, 0.0f);
			v[RB].mPos = Vector3(r, b, 0.0f);
		}
		{
			float w = float(mTexture->GetWidth());
			float h = float(mTexture->GetHeight());
			float l = mRect.x / w;
			float t = mRect.y / h;
			float r = mRect.z / w;
			float b = mRect.w / h;
			v[LT].mUv = Vector2(l, t);
			v[RT].mUv = Vector2(r, t);
			v[LB].mUv = Vector2(l, b);
			v[RB].mUv = Vector2(r, b);
		}
		mVBuff->Copy(v);
		// 定数
		SpriteCommon::Constant c = {};
		c.mWvp = transform->GetWorld() * SpriteCommon::mProjMat;
		c.mColor = mColor;
		mCBuff->Copy(c);

		mVBuff->Bind(SpriteCommon::mCmdList);
		mCBuff->Bind(SpriteCommon::mCmdList, 0);
		mTexture->Bind(SpriteCommon::mCmdList, 1);
		SpriteCommon::mCmdList->DrawInstanced(4, 1, 0, 0);
	}
}

// Post Effect
void Sprite::Draw(const Vector2& size)
{
	MY_ASSERT(SpriteCommon::mCmdList);
	if (mTexture)
	{
		// 頂点
		SpriteCommon::Vertex v[4] = {};
		enum { LT, RT, LB, RB };
		{
			v[LT].mPos = Vector3(0.0f, 0.0f, 0.0f);
			v[RT].mPos = Vector3(size.x, 0.0f, 0.0f);
			v[LB].mPos = Vector3(0.0f, size.y, 0.0f);
			v[RB].mPos = Vector3(size.x, size.y, 0.0f);
		}
		{
			v[LT].mUv = Vector2(0.0f, 0.0f);
			v[RT].mUv = Vector2(1.0f, 0.0f);
			v[LB].mUv = Vector2(0.0f, 1.0f);
			v[RB].mUv = Vector2(1.0f, 1.0f);
		}
		mVBuff->Copy(v);
		// 定数
		SpriteCommon::Constant c = {};
		c.mWvp = SpriteCommon::mProjMat;
		c.mColor = mColor;
		mCBuff->Copy(c);

		mVBuff->Bind(SpriteCommon::mCmdList);
		mCBuff->Bind(SpriteCommon::mCmdList, 0);
		mTexture->Bind(SpriteCommon::mCmdList, 1);
		SpriteCommon::mCmdList->DrawInstanced(4, 1, 0, 0);
	}
}

// アクセッサ
void Sprite::SetTexture(Texture* texture)
{
	mTexture = texture;
	if (mTexture)
	{
		Vector4 rect = {};
		rect.x = 0.0f;
		rect.y = 0.0f;
		rect.z = float(mTexture->GetWidth());
		rect.w = float(mTexture->GetHeight());
		SetRect(rect);
	}
}
