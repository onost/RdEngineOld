#pragma once
#include <cstdint>
#include <string>
#include <Windows.h>

// TODO: フルスクリーン
// TODO: 解像度

// ウィンドウ
class Window
{
public:
	// ウィンドウプロシージャ
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void Initialize();
	void Terminate();
	// メッセージを処理
	bool ProcessMessage();

	// タイトルを変更
	void SetTitle(const std::string& title);

	// アクセッサ
	HINSTANCE GetHInst() const { return mHInst; }
	HWND GetHWnd() const { return mHWnd; }

public:
	// ウィンドウの幅
	static const uint32_t kWidth;
	// ウィンドウの高さ
	static const uint32_t kHeight;
private:
	// タイトルバーの文字
	std::string mTitle;
	// インスタンスハンドル
	HINSTANCE mHInst;
	// ウィンドウハンドル
	HWND mHWnd;
};
