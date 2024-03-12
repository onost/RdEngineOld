#pragma once
#include "Component.h"
#include "Core/ConstantBuffer.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "Vector3.h"
#include <d3d12.h>

// トランスフォーム
class Transform : public Component
{
public:
	Transform(Actor* owner);
	void UpdateLocal();
	void UpdateWorld(Transform* parent = nullptr);
	void RenderForDev(Primitive* prim);
	void Bind(ID3D12GraphicsCommandList* cmdList, uint32_t rootParamIdx);

	void Copy(Matrix4 world);

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
	Type GetType() const override { return Type::Transform; }
	const Matrix4& GetLocal() const { return mLocal; }
	const Matrix4& GetWorld() const { return mWorld; }
	void SetWorld(const Matrix4& world) { mWorld = world; }
	const Matrix4& GetTWorld() const { return mTWorld; }

public:
	Vector3 mScale;
	Quaternion mRotation;
	Vector3 mPosition;
private:
	std::unique_ptr<ConstantBuffer> mCBuff;
	Matrix4 mLocal;
	Matrix4 mWorld;
	Matrix4 mTWorld;
};
