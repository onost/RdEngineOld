#include "Shader.h"
#include "Core/GraphicsEngine.h"
#include "Helper/MyAssert.h"
#include <format>

#pragma comment(lib, "dxcompiler.lib")

Microsoft::WRL::ComPtr<IDxcUtils> Shader::mDxcUtils = nullptr;
Microsoft::WRL::ComPtr<IDxcCompiler3> Shader::mDxcCompiler = nullptr;
Microsoft::WRL::ComPtr<IDxcIncludeHandler> Shader::mIncludeHandler = nullptr;

void Shader::Initialize()
{
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&mDxcUtils));
	MyAssert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&mDxcCompiler));
	MyAssert(SUCCEEDED(hr));
	hr = mDxcUtils->CreateDefaultIncludeHandler(&mIncludeHandler);
	MyAssert(SUCCEEDED(hr));
}

bool Shader::LoadVs(const std::string& filePath)
{
	return Compile(filePath, "vs_6_0");
}

bool Shader::LoadGs(const std::string& filePath)
{
	return Compile(filePath, "gs_6_0");
}

bool Shader::LoadPs(const std::string& filePath)
{
	return Compile(filePath, "ps_6_0");
}

// シェーダをコンパイル
bool Shader::Compile(const std::string& filePath, const std::string& profile)
{
	std::wstring path = Helper::ConvertToWstr(filePath);
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> encoding = nullptr;
	HRESULT hr = mDxcUtils->LoadFile(path.c_str(), nullptr, &encoding);
	if (FAILED(hr))
	{
		return false;
	}

	// コンパイル
	DxcBuffer source = {};
	source.Ptr = encoding->GetBufferPointer();
	source.Size = encoding->GetBufferSize();
	source.Encoding = DXC_CP_UTF8;
	// コンパイルオプション
	auto p = Helper::ConvertToWstr(profile);
	LPCWSTR arguments[] =
	{
		path.c_str(),
		L"-E", L"main",
		L"-T", p.c_str(),
		L"-Zi", L"-Qembed_debug",
		L"-Od",
		L"-Zpr"
	};
	Microsoft::WRL::ComPtr<IDxcResult> result = nullptr;
	hr = mDxcCompiler->Compile(
		&source, arguments, _countof(arguments),
		mIncludeHandler.Get(), IID_PPV_ARGS(&result));
	MyAssert(SUCCEEDED(hr));

	Microsoft::WRL::ComPtr<IDxcBlobUtf8> error = nullptr;
	result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&error), nullptr);
	if (error && error->GetStringLength() != 0)
	{
		Helper::WriteToConsole(std::format("{}", error->GetStringPointer()));
		return false;
	}

	hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&mBlob), nullptr);
	MyAssert(SUCCEEDED(hr));

	return true;
}
