#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <string>
#include <wrl.h>

// シェーダ
class Shader
{
public:
	static void Initialize();

	bool LoadVs(const std::string& filePath);
	bool LoadGs(const std::string& filePath);
	bool LoadPs(const std::string& filePath);
	// シェーダをコンパイル
	bool Compile(const std::string& filePath, const std::string& profile);

	IDxcBlob* Get() const { return mBlob.Get(); }

private:
	static Microsoft::WRL::ComPtr<IDxcUtils> mDxcUtils;
	static Microsoft::WRL::ComPtr<IDxcCompiler3> mDxcCompiler;
	static Microsoft::WRL::ComPtr<IDxcIncludeHandler> mIncludeHandler;
	Microsoft::WRL::ComPtr<IDxcBlob> mBlob;
};
