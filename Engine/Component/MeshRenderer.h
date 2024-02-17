#pragma once
#include "Graphics/Model/Model.h"
#include "Graphics/Model/ModelCommon.h"
#include "RendererComponent.h"

class Material;
//class Model;

// メッシュレンダラー
class MeshRenderer : public RendererComponent
{
public:
	MeshRenderer(Actor* owner);
	virtual ~MeshRenderer();

	void Draw() override;

	// ==================================================
	// json
	// ==================================================
	void Load(const nlohmann::json& json) override;
	void Save(nlohmann::json& json) override;

	// ==================================================
	// 開発用
	// ==================================================
	virtual void UpdateForDev() override;

	// ==================================================
	// ヘルパー関数
	// ==================================================
	Type GetType() const override { return Type::MeshRenderer; }
	Model* GetModel() const { return mModel; }
	ModelCommon::Type GetShaderType() const { return mShaderType; }
	void SetModel(Model* model);// cpp
	void SetShaderType(ModelCommon::Type shaderType) { mShaderType = shaderType; }

protected:
	// ==================================================
	// 開発用
	// ==================================================
	void UpdateModel();

protected:
	Model* mModel;
	ModelCommon::Type mShaderType;// シェーダタイプ
	// 開発用
	std::string mModelName;
	Material* mMatForDev;
	std::string mTexturePath;
};
