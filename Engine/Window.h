#pragma once
#include <cstdint>
#include <string>
#include <Windows.h>

class Window
{
public:
	// ウィンドウプロシージャ
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void Initialize();
	void Terminate();
	// メッセージを処理
	bool ProcessMessage();

	HINSTANCE GetHInst() const { return mHInst; }
	HWND GetHWnd() const { return mHWnd; }

public:
	// ウィンドウサイズ
	static const uint32_t kWidth;
	static const uint32_t kHeight;
private:
	static std::string kTitle;// タイトルバー
	HINSTANCE mHInst;
	HWND mHWnd;
};
