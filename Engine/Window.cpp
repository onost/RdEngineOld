#include "Window.h"
#include "Helper/Helper.h"
#include "RdEngine.h"
#include <ImGui/imgui_impl_win32.h>

// ウィンドウの幅
const uint32_t Window::kWidth = 1280;
// ウィンドウの高さ
const uint32_t Window::kHeight = 720;

// ImGuiのウィンドウプロシージャ
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
	// タイトル
	mTitle = std::format(
		"{} Ver.{}.{}.{}\n",
		RdEngine::kName,
		RdEngine::kVersion[0],
		RdEngine::kVersion[1],
		RdEngine::kVersion[2]);
	//mTitle = "Galaxy";

	// インスタンスハンドルを取得
	mHInst = GetModuleHandle(nullptr);
	// ウィンドウクラスを設定
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = mHInst;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = L"RdWndClass";
	RegisterClassEx(&wc);
	// クライアント領域のサイズからウィンドウのサイズを計算
	RECT rc = { 0,0,kWidth,kHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

	auto title = Helper::ConvertToWstr(mTitle);
	// ウィンドウを作成
	mHWnd = CreateWindow(
		wc.lpszClassName,
		title.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		mHInst,
		nullptr);
	// ウィンドウを表示
	ShowWindow(mHWnd, SW_SHOW);
}

void Window::Terminate()
{
	CloseWindow(mHWnd);
}

// メッセージを処理
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

void Window::SetTitle(const std::string& title)
{
	mTitle = title;
	auto t = Helper::ConvertToWstr(mTitle);
	SetWindowText(mHWnd, t.c_str());
}
