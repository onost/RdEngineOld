#pragma once
#include "Component.h"
#include <d3d12.h>

// レンダラーコンポーネント
class RendererComponent : public Component
{
public:
	RendererComponent(Actor* owner);
	virtual ~RendererComponent();

	virtual void Draw(ID3D12GraphicsCommandList* /*cmdList*/) {};
	virtual void Draw() {};

	// ==================================================
	// json
	// ==================================================
	virtual void Load(const nlohmann::json& json);
	virtual void Save(nlohmann::json& json);

	// ==================================================
	// 開発用
	// ==================================================
	virtual void UpdateForDev();

	// ==================================================
	// ヘルパー関数
	// ==================================================
	bool GetIsVisible() const { return mIsVisible; }
	void SetIsVisible(bool isVisible) { mIsVisible = isVisible; }

protected:
	bool mIsVisible;
};
