#pragma once
#include <string>

namespace Helper
{
	void WriteToConsole(const std::string& str);
	void WriteToOutput(const std::string& str);

	std::wstring ConvertToWstr(const std::string& str);
	std::string ConvertToStr(const std::wstring& str);

	std::string GetFileName(const std::string& filePath);
	std::string GetExtension(const std::string& filePath);
	std::string RemoveExtension(const std::string& fileName);
}
