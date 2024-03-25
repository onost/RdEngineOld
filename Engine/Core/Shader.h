#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <string>
#include <wrl.h>

// シェーダ
class Shader
{
public:
	Shader();

	bool CompileVs(const std::string& filePath);
	bool CompileGs(const std::string& filePath);
	bool CompilePs(const std::string& filePath);

	IDxcBlob* GetBlob() const { return mBlob.Get(); }

private:
	bool CheckResult();

private:
	std::string mFilePath;
	// シェーダオブジェクト
	Microsoft::WRL::ComPtr<IDxcBlob> mBlob;
};
