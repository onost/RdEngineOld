#include "Shader.h"
#include "GraphicsEngine.h"
#include "Helper/Helper.h"
#include <format>

Shader::Shader()
	: mFilePath()
	, mBlob(nullptr)
{}

// Vertex Shader
bool Shader::CompileVs(const std::string& filePath)
{
	mFilePath = filePath;
	mBlob = gGraphicsEngine->CompileShader(filePath, "vs_6_0");
	return CheckResult();
}

// Geometry Shader
bool Shader::CompileGs(const std::string& filePath)
{
	mFilePath = filePath;
	mBlob = gGraphicsEngine->CompileShader(filePath, "gs_6_0");
	return CheckResult();
}

// Pixel Shader
bool Shader::CompilePs(const std::string& filePath)
{
	mFilePath = filePath;
	mBlob = gGraphicsEngine->CompileShader(filePath, "ps_6_0");
	return CheckResult();
}

bool Shader::CheckResult()
{
	if (mBlob)
	{
		Helper::WriteToOutputWindow(
			std::format("Success: Compile \"{}\"\n", mFilePath));
		return true;
	}
	else
	{
		Helper::WriteToOutputWindow(
			std::format("Failure: Compile \"{}\"\n", mFilePath));
		return false;
	}
}
