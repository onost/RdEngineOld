#pragma once
#include "Component.h"
#include "Graphics/Camera.h"
#include <memory>

// カメラコンポーネント
class CameraComponent : public Component
{
public:
	CameraComponent(Actor* owner);
	virtual ~CameraComponent();
	void OnUpdateWorld() override;
	// メインカメラとしてセット
	void SetMainCamera();

	// ==================================================
	// json
	// ==================================================
	void Load(const nlohmann::json& json) override;
	void Save(nlohmann::json& json) override;

	// ==================================================
	// 開発用
	// ==================================================
	void UpdateForDev() override;

	// ==================================================
	// ヘルパー関数
	// ==================================================
	Type GetType() const override { return Type::CameraComponent; }
	Camera* GetCamera() const { return mCamera.get(); }
	void SetIsMain(bool isMain) { mIsMain = isMain; }

private:
	std::unique_ptr<Camera> mCamera;
	bool mIsMain;// メインカメラか
};
