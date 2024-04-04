#include "Input/InputSystem.h"
#include "Helper/MyAssert.h"
#include "Window.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "dxguid.lib")

InputSystem::InputSystem()
	: mHWnd(nullptr)
	, mDirectInput(nullptr)
	, mKeyDevice(nullptr)
	, mMouseDevice(nullptr)
	, mState()
{}

void InputSystem::Initialize(Window* window)
{
	MY_ASSERT(window);
	mHWnd = window->GetHWnd();

	// DirectInputオブジェクトを作成
	HRESULT hr = DirectInput8Create(
		window->GetHInst(), DIRECTINPUT_VERSION, IID_IDirectInput8, static_cast<void**>(&mDirectInput), nullptr);
	MY_ASSERT(SUCCEEDED(hr));

	// キーボード
	hr = mDirectInput->CreateDevice(GUID_SysKeyboard, &mKeyDevice, nullptr);
	MY_ASSERT(SUCCEEDED(hr));
	hr = mKeyDevice->SetDataFormat(&c_dfDIKeyboard);
	MY_ASSERT(SUCCEEDED(hr));
	hr = mKeyDevice->SetCooperativeLevel(mHWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	MY_ASSERT(SUCCEEDED(hr));

	// マウス
	hr = mDirectInput->CreateDevice(GUID_SysMouse, &mMouseDevice, nullptr);
	MY_ASSERT(SUCCEEDED(hr));
	hr = mMouseDevice->SetDataFormat(&c_dfDIMouse2);
	MY_ASSERT(SUCCEEDED(hr));
	hr = mMouseDevice->SetCooperativeLevel(mHWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	MY_ASSERT(SUCCEEDED(hr));

	// 入力開始
	mKeyDevice->Acquire();
	mMouseDevice->Acquire();
}

void InputSystem::Terminate()
{
	// 入力終了
	mMouseDevice->Unacquire();
	mKeyDevice->Unacquire();
}

void InputSystem::Update()
{
	// キーボード
	auto& keyboard = mState.mKeyboard;
	memcpy(keyboard.mPrev, keyboard.mCurr, sizeof(keyboard.mCurr));
	ZeroMemory(keyboard.mCurr, sizeof(keyboard.mCurr));
	HRESULT hr = mKeyDevice->GetDeviceState(sizeof(keyboard.mCurr), keyboard.mCurr);
	if (FAILED(hr))
	{
		mKeyDevice->Acquire();
		mKeyDevice->GetDeviceState(sizeof(keyboard.mCurr), keyboard.mCurr);
	}

	// マウス
	auto& mouse = mState.mMouse;
	mouse.mPrev = mouse.mCurr;
	hr = mMouseDevice->GetDeviceState(sizeof(mouse.mCurr), &mouse.mCurr);
	if (FAILED(hr))
	{
		mMouseDevice->Acquire();
		mMouseDevice->GetDeviceState(sizeof(mouse.mCurr), &mouse.mCurr);
	}
	// マウス座標
	POINT position = {};
	GetCursorPos(&position);
	ScreenToClient(mHWnd, &position);
	mouse.mPosition.x = float(position.x);
	mouse.mPosition.y = float(position.y);

	// ゲームパッド
	mState.mGamepad.mPrev = mState.mGamepad.mCurr;
	XInputGetState(0, &mState.mGamepad.mCurr);
}
