#pragma once
#include <string>

// ヘルパー関数群
namespace Helper
{
	// 出力ウィンドウへ出力
	void WriteToOutputWindow(const std::string& str);

	// 文字列を変換
	std::wstring ConvertToWstr(const std::string& str);
	std::string ConvertToStr(const std::wstring& wstr);

	// ファイルパスからファイル名を抽出
	std::string ExtractFileName(const std::string& filePath);
	// ファイル名から拡張子を抽出
	std::string ExtractExtension(const std::string& fileName);
	// ファイル名から拡張子を除外
	std::string ExcludeExtension(const std::string& fileName);
}
