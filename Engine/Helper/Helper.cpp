#include "Helper.h"
#include <Windows.h>

namespace Helper
{
	// 出力ウィンドウへ出力
	void WriteToOutputWindow(const std::string& str)
	{
		OutputDebugStringA(str.c_str());
	}

	// マルチバイト文字をワイド文字へ変換
	std::wstring ConvertToWstr(const std::string& str)
	{
		if (str.empty())
		{
			return std::wstring();
		}

		// 必要なサイズを取得
		auto sizeNeeded = MultiByteToWideChar(
			CP_UTF8, 0, str.data(), int(str.size()), nullptr, 0);
		if (sizeNeeded < 1)
		{
			return std::wstring();
		}

		std::wstring result(sizeNeeded, 0);
		// ワイド文字へ変換
		MultiByteToWideChar(
			CP_UTF8, 0, str.data(), int(str.size()), result.data(), sizeNeeded);
		return result;
	}

	// ワイド文字をマルチバイト文字へ変換
	std::string ConvertToStr(const std::wstring& wstr)
	{
		if (wstr.empty())
		{
			return std::string();
		}

		// 必要なサイズを取得
		auto sizeNeeded = WideCharToMultiByte(
			CP_UTF8, 0, wstr.data(), int(wstr.size()), nullptr, 0, nullptr, nullptr);
		if (sizeNeeded < 1)
		{
			return std::string();
		}

		std::string result(sizeNeeded, 0);
		// マルチバイト文字へ変換
		WideCharToMultiByte(
			CP_UTF8, 0, wstr.data(), int(wstr.size()), result.data(), sizeNeeded, nullptr, nullptr);
		return result;
	}

	// ファイルパスからファイル名を抽出
	std::string ExtractFileName(const std::string& filePath)
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

	// ファイル名から拡張子を抽出
	std::string ExtractExtension(const std::string& fileName)
	{
		auto pos = fileName.rfind('.');
		if (pos != std::string::npos)
		{
			return fileName.substr(pos + 1, fileName.size() - pos);
		}
		return fileName;
	}

	// ファイル名から拡張子を除外
	std::string ExcludeExtension(const std::string& fileName)
	{
		auto pos = fileName.rfind('.');
		if (pos != std::string::npos)
		{
			return fileName.substr(0, pos);
		}
		return fileName;
	}
}
