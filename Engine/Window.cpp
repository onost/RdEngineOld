#include "Window.h"
#include "Helper/Helper.h"
#include "RdEngine.h"
#include <ImGui/imgui_impl_win32.h>

// ウィンドウサイズ
//const uint32_t Window::kWidth = 1920;
//const uint32_t Window::kHeight = 1080;
const uint32_t Window::kWidth = 1280;
const uint32_t Window::kHeight = 720;
// タイトルバー
std::string Window::kTitle = "None";

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ウィンドウプロシージャ
LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Window::Initialize()
{
#ifdef _DEBUG
	kTitle = std::format("{} Ver.{}.{}.{}",
		gEngine->kName, gEngine->kVersion[0], gEngine->kVersion[1], gEngine->kVersion[2]);
#else
	kTitle = "Game title";
#endif
	mHInst = GetModuleHandle(nullptr);
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = mHInst;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = L"RdWndClass";
	RegisterClassEx(&wc);

	RECT wrc = { 0,0,kWidth,kHeight };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウを作成
	mHWnd = CreateWindow(
		wc.lpszClassName,
		Helper::ConvertToWstr(kTitle).c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		mHInst,
		nullptr);

	ShowWindow(mHWnd, SW_SHOW);// 表示
}

void Window::Terminate()
{
	CloseWindow(mHWnd);
}

bool Window::ProcessMessage()
{
	MSG msg = {};
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msg.message == WM_QUIT)
	{
		return true;
	}
	return false;
}
