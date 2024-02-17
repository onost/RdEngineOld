#pragma once
#include "Color.h"
#include "Core/ConstantBuffer.h"
#include "Core/VertexBuffer.h"
#include "Vector2.h"
#include "Vector4.h"
#include <d3d12.h>
#include <memory>

class Texture;
class Transform;

class Sprite
{
public:
	Sprite();
	void Create(Texture* texture = nullptr);
	void Draw(Transform* transform);
	void Draw(const Vector2& size);

	Texture* GetTexture() const { return mTexture; }
	const Vector2& GetPivot() const { return mPivot; }
	bool GetIsFlipX() const { return mIsFlipX; }
	bool GetIsFlipY() const { return mIsFlipY; }
	const Vector4& GetRect() const { return mRect; }
	const Color& GetColor() const { return mColor; }
	void SetTexture(Texture* texture);// cpp
	void SetPivot(const Vector2& pivot) { mPivot = pivot; }
	void SetIsFlipX(bool isFlipX) { mIsFlipX = isFlipX; }
	void SetIsFlipY(bool isFlipY) { mIsFlipY = isFlipY; }
	void SetRect(const Vector4& rect) { mRect = rect; }
	void SetColor(const Color& color) { mColor = color; }

private:
	std::unique_ptr<VertexBuffer> mVBuff;
	std::unique_ptr<ConstantBuffer> mCBuff;
	Texture* mTexture;
	Vector2 mPivot;
	bool mIsFlipX;
	bool mIsFlipY;
	Vector4 mRect;
	Color mColor;
};
