#pragma once
#include "Color.h"
#include "Core/ConstantBuffer.h"
#include <memory>
#include <string>

class Renderer;
class Texture;

class Material
{
	friend class ObjLoader;
private:
	// 定数
	struct Constant
	{
		Color mBaseColor;
		float mSpecular;
	};

public:
	Material();
	void Create(Renderer* renderer);
	void Bind(ID3D12GraphicsCommandList* cmdList,
		uint32_t matRootParamIdx, uint32_t texRootParamIdx);

	// Getter
	const std::string& GetName() const { return mName; }
	Texture* GetTexture() const { return mTexture; }
	const Color& GetBaseColor() const { return mBaseColor; }
	float GetSpecular() const { return mSpecular; }
	// Setter
	//void SetName(const std::string& name) { mName = name; }
	//void SetTexturePath(const std::string& texturePath) { mTexturePath = texturePath; }
	void SetTexture(Texture* texture) { mTexture = texture; }
	void SetBaseColor(const Color& baseColor) { mBaseColor = baseColor; }
	void SetSpecular(float specular) { mSpecular = specular; }

private:
	std::unique_ptr<ConstantBuffer> mCBuff;
	std::string mName;
	std::string mTexturePath;
	Texture* mTexture;
	// Constant
	Color mBaseColor;
	float mSpecular;
};
