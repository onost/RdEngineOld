#include "Helper.h"
#include <Windows.h>

namespace Helper
{
	void WriteToConsole(const std::string& str)
	{
		printf(str.c_str());
	}

	void WriteToOutput(const std::string& str)
	{
		OutputDebugStringA(str.c_str());
	}

	std::wstring ConvertToWstr(const std::string& str)
	{
		if (str.empty())
		{
			return std::wstring();
		}
		auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.data(), int(str.size()), nullptr, 0);
		if (sizeNeeded < 1)
		{
			return std::wstring();
		}
		std::wstring result(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.data(), int(str.size()), result.data(), sizeNeeded);
		return result;
	}

	std::string ConvertToStr(const std::wstring& str)
	{
		if (str.empty())
		{
			return std::string();
		}
		auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), int(str.size()), nullptr, 0, nullptr, nullptr);
		if (sizeNeeded < 1)
		{
			return std::string();
		}
		std::string result(sizeNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, str.data(), int(str.size()), result.data(), sizeNeeded, nullptr, nullptr);
		return result;
	}

	std::string GetFileName(const std::string& filePath)
	{
		auto pos = filePath.rfind('\\');
		if (pos != std::string::npos)
		{
			return filePath.substr(pos + 1, filePath.size() - pos - 1);
		}
		pos = filePath.rfind('/');
		if (pos != std::string::npos)
		{
			return filePath.substr(pos + 1, filePath.size() - pos - 1);
		}
		return filePath;
	}

	std::string GetExtension(const std::string& filePath)
	{
		auto pos = filePath.rfind('.');
		if (pos != std::string::npos)
		{
			return filePath.substr(pos + 1, filePath.size() - pos);
		}
		return filePath;
	}

	std::string RemoveExtension(const std::string& fileName)
	{
		auto pos = fileName.rfind(".");
		if (pos != std::string::npos)
		{
			return fileName.substr(0, pos);
		}
		return fileName;
	}
}
