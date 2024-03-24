#pragma once
#include "Core/ConstantBuffer.h"
#include "CircleShadow.h"
#include "DirectionalLight.h"
#include "Helper/MyAssert.h"
#include "PointLight.h"
#include "SpotLight.h"
#include <memory>
#include <nlohmann/json.hpp>

// ライト管理
class LightManager
{
public:
	static const uint32_t kDirectionalLightCount = 5;
	static const uint32_t kPointLightCount = 5;
	static const uint32_t kSpotLightCount = 5;
	static const uint32_t kCircleShadowCount = 10;

public:
	void Initialize();
	//void Terminate();

	void Update();
	void Bind(ID3D12GraphicsCommandList* cmdList, uint32_t lightRootParamIdx);

	// ==================================================
	// json
	// ==================================================
	void LoadLevel(const nlohmann::json& json);
	void SaveLevel(nlohmann::json& json);

	// ==================================================
	// 開発用
	// ==================================================
	void UpdateForDev();

private:
	struct Constant
	{
		DirectionalLight mDirectionalLights[kDirectionalLightCount];
		PointLight mPointLights[kPointLightCount];
		SpotLight mSpotLights[kSpotLightCount];
		CircleShadow mCircleShadows[kCircleShadowCount];
	};

	std::unique_ptr<ConstantBuffer> mCBuff;
	//Constant* mData;
	DirectionalLight mDirectionalLights[kDirectionalLightCount];
	PointLight mPointLights[kPointLightCount];
	SpotLight mSpotLights[kSpotLightCount];
	CircleShadow mCircleShadows[kCircleShadowCount];

public:
#pragma region Accessor
	// Directional Light
	const Color& GetDirectionalLightColor(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < kDirectionalLightCount);
		return mDirectionalLights[i].mColor;
	}
	const Vector3& GetDirectionalLightDirection(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < kDirectionalLightCount);
		return mDirectionalLights[i].mDirection;
	}
	float GetDirectionalLightIntensity(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < kDirectionalLightCount);
		return mDirectionalLights[i].mIntensity;
	}

	// Point Light
	const Color& GetPointLightColor(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < kPointLightCount);
		return mPointLights[i].mColor;
	}
	const Vector3& GetPointLightPosition(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < kPointLightCount);
		return mPointLights[i].mPosition;
	}
	float GetPointLightIntensity(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < kPointLightCount);
		return mPointLights[i].mIntensity;
	}
	float GetPointLightRadius(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < kPointLightCount);
		return mPointLights[i].mRadius;
	}
	float GetPointLightDecay(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < kPointLightCount);
		return mPointLights[i].mDecay;
	}

	// Spot Light
	const Color& GetSpotLightColor(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < kSpotLightCount);
		return mSpotLights[i].mColor;
	}
	const Vector3& GetSpotLightDirection(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < kSpotLightCount);
		return mSpotLights[i].mDirection;
	}
	float GetSpotLightIntensity(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < kSpotLightCount);
		return mSpotLights[i].mIntensity;
	}
	const Vector3& GetSpotLightPosition(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < kSpotLightCount);
		return mSpotLights[i].mPosition;
	}
	float GetSpotLightRadius(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < kSpotLightCount);
		return mSpotLights[i].mRadius;
	}
	float GetSpotLightDecay(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < kSpotLightCount);
		return mSpotLights[i].mDecay;
	}
	float GetSpotLightInner(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < kSpotLightCount);
		return mSpotLights[i].mInner;
	}
	float GetSpotLightOuter(uint32_t i)
	{
		MY_ASSERT(i >= 0 && i < kSpotLightCount);
		return mSpotLights[i].mOuter;
	}

	// Directional Light
	void SetDirectionalLightColor(uint32_t i, const Color& color)
	{
		MY_ASSERT(i >= 0 && i < kDirectionalLightCount);
		mDirectionalLights[i].mColor = color;
	}
	void SetDirectionalLightDirection(uint32_t i, const Vector3& direction)
	{
		MY_ASSERT(i >= 0 && i < kDirectionalLightCount);
		mDirectionalLights[i].mDirection = direction;
	}
	void SetDirectionalLightIntensity(uint32_t i, float intensity)
	{
		MY_ASSERT(i >= 0 && i < kDirectionalLightCount);
		mDirectionalLights[i].mIntensity = intensity;
	}

	// Point Light
	void SetPointLightColor(uint32_t i, const Color& color)
	{
		MY_ASSERT(i >= 0 && i < kPointLightCount);
		mPointLights[i].mColor = color;
	}
	void SetPointLightPosition(uint32_t i, const Vector3& position)
	{
		MY_ASSERT(i >= 0 && i < kPointLightCount);
		mPointLights[i].mPosition = position;
	}
	void SetPointLightIntensity(uint32_t i, float intensity)
	{
		MY_ASSERT(i >= 0 && i < kPointLightCount);
		mPointLights[i].mIntensity = intensity;
	}
	void SetPointLightRadius(uint32_t i, float radius)
	{
		MY_ASSERT(i >= 0 && i < kPointLightCount);
		mPointLights[i].mRadius = radius;
	}
	void SetPointLightDecay(uint32_t i, float decay)
	{
		MY_ASSERT(i >= 0 && i < kPointLightCount);
		mPointLights[i].mDecay = decay;
	}

	// Circle Shadow
	void SetCircleShadowDirection(uint32_t i, const Vector3& direction)
	{
		MY_ASSERT(i >= 0 && i < kCircleShadowCount);
		mCircleShadows[i].mDirection = direction;
	}
	void SetCircleShadowIntensity(uint32_t i, float intensity)
	{
		MY_ASSERT(i >= 0 && i < kCircleShadowCount);
		mCircleShadows[i].mIntensity = intensity;
	}
	void SetCircleShadowPosition(uint32_t i, const Vector3& position)
	{
		MY_ASSERT(i >= 0 && i < kCircleShadowCount);
		mCircleShadows[i].mPosition = position;
	}
	void SetCircleShadowRadius(uint32_t i, float radius)
	{
		MY_ASSERT(i >= 0 && i < kCircleShadowCount);
		mCircleShadows[i].mRadius = radius;
	}
	void SetCircleShadowDecay(uint32_t i, float decay)
	{
		MY_ASSERT(i >= 0 && i < kCircleShadowCount);
		mCircleShadows[i].mDecay = decay;
	}
	void SetCircleShadowInner(uint32_t i, float inner)
	{
		MY_ASSERT(i >= 0 && i < kCircleShadowCount);
		mCircleShadows[i].mInner = inner;
	}
	void SetCircleShadowOuter(uint32_t i, float outer)
	{
		MY_ASSERT(i >= 0 && i < kCircleShadowCount);
		mCircleShadows[i].mOuter = outer;
	}
#pragma endregion Accessor
};
