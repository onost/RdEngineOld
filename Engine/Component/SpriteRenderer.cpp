#include "SpriteRenderer.h"
#include "Actor/Actor.h"
#include "Editor.h"
#include "Graphics/Renderer.h"
#include "Helper/JsonHelper.h"
#include "Scene/Scene.h"

SpriteRenderer::SpriteRenderer(Actor* owner, uint32_t drawOrder)
	: RendererComponent(owner)
	, mDrawOrder(drawOrder)
	, mBlend(SpriteCommon::Blend::Normal)
	, mSprite(nullptr)
	, mTexturePath()
{
	mSprite = std::make_unique<Sprite>();
	mSprite->Create();
	mOwner->GetScene()->GetRenderer()->AddSprite(this);
}

SpriteRenderer::~SpriteRenderer()
{
	mOwner->GetScene()->GetRenderer()->RemoveSprite(this);
}

void SpriteRenderer::Draw()
{
	if (mIsVisible)
	{
		SpriteCommon::SetBlend(mBlend);
		mSprite->Draw(mOwner->mTransform);
	}
}

// ==================================================
// json
// ==================================================

void SpriteRenderer::Load(const nlohmann::json& json)
{
	RendererComponent::Load(json);
	// Draw Order
	JsonHelper::GetUint(json, "Draw Order", mDrawOrder);
	// Blend
	uint32_t blend = 0;
	JsonHelper::GetUint(json, "Blend", blend);
	mBlend = SpriteCommon::Blend(blend);
	// Texture
	std::string texturePath;
	if (JsonHelper::GetString(json, "Texture", texturePath))
	{
		auto renderer = mOwner->GetScene()->GetRenderer();
		auto texture = renderer->GetTexture(texturePath);
		if (texture)
		{
			mSprite->SetTexture(texture);
			mTexturePath = texture->GetFilePath();
		}
	}
	// Pivot
	Vector2 pivot;
	JsonHelper::GetVector2(json, "Pivot", pivot);
	mSprite->SetPivot(pivot);
	// Is Flip X,Y
	bool isFlipX, isFlipY;
	JsonHelper::GetBool(json, "Is Flip X", isFlipX);
	JsonHelper::GetBool(json, "Is Flip Y", isFlipY);
	mSprite->SetIsFlipX(isFlipX);
	mSprite->SetIsFlipY(isFlipY);
	// Rect
	Vector4 rect;
	JsonHelper::GetVector4(json, "Rect", rect);
	mSprite->SetRect(rect);
	// Color
	Color color;
	JsonHelper::GetColor(json, "Color", color);
	mSprite->SetColor(color);

	auto renderer = mOwner->GetScene()->GetRenderer();
	renderer->SortSprites(this);
}

void SpriteRenderer::Save(nlohmann::json& json)
{
	RendererComponent::Save(json);
	JsonHelper::SetUint(json, "Draw Order", mDrawOrder);
	JsonHelper::SetUint(json, "Blend", uint32_t(mBlend));
	auto texture = mSprite->GetTexture();
	if (texture)
	{
		JsonHelper::SetString(json, "Texture", texture->GetFilePath());
	}
	JsonHelper::SetVector2(json, "Pivot", mSprite->GetPivot());
	JsonHelper::SetBool(json, "Is Flip X", mSprite->GetIsFlipX());
	JsonHelper::SetBool(json, "Is Flip Y", mSprite->GetIsFlipY());
	JsonHelper::SetVector4(json, "Rect", mSprite->GetRect());
	JsonHelper::SetColor(json, "Color", mSprite->GetColor());
}

// ==================================================
// 開発用
// ==================================================

void SpriteRenderer::UpdateForDev()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Sprite Renderer"))
	{
		RendererComponent::UpdateForDev();
		// Draw Order
		if (ImGui::InputScalar("Draw Order", ImGuiDataType_U32, &mDrawOrder))
		{
			auto renderer = mOwner->GetScene()->GetRenderer();
			renderer->SortSprites(this);
		}
		// Blend
		Editor::Combo("Blend", mBlend);
		// Texture
		ImGui::InputText("Texture Path", &mTexturePath);
		if (ImGui::BeginDragDropTarget())// ドロップ
		{
			if (auto payload = ImGui::AcceptDragDropPayload("TEXTURE_PAYLOAD"))
			{
				auto texture = (Texture**)(payload->Data);
				mSprite->SetTexture(*texture);
				mTexturePath = (*texture)->GetFilePath();
			}
			ImGui::EndDragDropTarget();
		}
		if (ImGui::Button("Load"))
		{
			auto renderer = mOwner->GetScene()->GetRenderer();
			auto texture = renderer->GetTexture(mTexturePath);
			if (texture)
			{
				mSprite->SetTexture(texture);
			}
		}
		// Pivot
		Vector2 pivot = mSprite->GetPivot();
		if (ImGui::DragFloat2("Pivot", &pivot.x, 0.001f, 0.0f, 1.0f))// 0.0～1.0
		{
			mSprite->SetPivot(pivot);
		}
		// Is Flip X,Y
		bool isFlipX = mSprite->GetIsFlipX();
		bool isFlipY = mSprite->GetIsFlipY();
		if (ImGui::Checkbox("Is Flip X", &isFlipX))
		{
			mSprite->SetIsFlipX(isFlipX);
		}
		if (ImGui::Checkbox("Is Flip Y", &isFlipY))
		{
			mSprite->SetIsFlipY(isFlipY);
		}
		// Rect
		auto texture = mSprite->GetTexture();
		if (texture)
		{
			// rect.x: 左
			// rect.y: 上
			// rect.z: 右
			// rect.w: 下
			Vector4 rect = mSprite->GetRect();
			if (ImGui::DragFloat("Rect.x1", &rect.x, 1.0f, 0.0f, rect.z))
			{
				mSprite->SetRect(rect);
			}
			if (ImGui::DragFloat("Rect.y1", &rect.y, 1.0f, 0.0f, rect.w))
			{
				mSprite->SetRect(rect);
			}
			if (ImGui::DragFloat("Rect.x2", &rect.z, 1.0f, rect.x, float(texture->GetWidth())))
			{
				mSprite->SetRect(rect);
			}
			if (ImGui::DragFloat("Rect.y2", &rect.w, 1.0f, rect.y, float(texture->GetHeight())))
			{
				mSprite->SetRect(rect);
			}
		}
		// Color
		Color color = mSprite->GetColor();
		if (ImGui::ColorEdit4("Color", &color.r))
		{
			mSprite->SetColor(color);
		}
		ImGui::TreePop();
	}
}

// v.z = 0.0f
Vector2 operator*(const Vector2& v, const Matrix4& m)
{
	return Vector2(
		v.x * m.m[0][0] + v.y * m.m[1][0] + m.m[3][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + m.m[3][1]);
}
void SpriteRenderer::RenderForDev(Primitive* primitive)
{
	Vector2 v[4];
	enum { LT, RT, LB, RB };
	Vector4 rect = mSprite->GetRect();
	float w = rect.z - rect.x;
	float h = rect.w - rect.y;
	Vector2 pivot = mSprite->GetPivot();
	float l = (0.0f - pivot.x) * w;
	float t = (0.0f - pivot.y) * h;
	float r = (1.0f - pivot.x) * w;
	float b = (1.0f - pivot.y) * h;
	if (mSprite->GetIsFlipX())
	{
		l = -l;
		r = -r;
	}
	if (mSprite->GetIsFlipY())
	{
		t = -t;
		b = -b;
	}
	Matrix4 world = mOwner->mTransform->GetWorld();
	v[LT] = Vector2(l, t) * world;
	v[RT] = Vector2(r, t) * world;
	v[LB] = Vector2(l, b) * world;
	v[RB] = Vector2(r, b) * world;
	primitive->DrawLine2(v[LT], v[RT], Color::kWhite);
	primitive->DrawLine2(v[RT], v[RB], Color::kWhite);
	primitive->DrawLine2(v[RB], v[LB], Color::kWhite);
	primitive->DrawLine2(v[LB], v[LT], Color::kWhite);
	Vector3 position = mOwner->mTransform->mPosition;
	primitive->DrawCircle(Vector2(position.x, position.y), 5.0f, Color::kWhite, false);
}

// ==================================================
// ヘルパー関数
// ==================================================

void SpriteRenderer::SetDrawOrder(uint32_t drawOrder)
{
	mDrawOrder = drawOrder;
	auto renderer = mOwner->GetScene()->GetRenderer();
	renderer->SortSprites(this);
}
