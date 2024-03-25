#include "Input/InputSystem.h"
#include "Helper/Helper.h"
#include "Helper/MyAssert.h"
#include "Window.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "dxguid.lib")

void InputSystem::Initialize(Window* window)
{
	MY_ASSERT(window);

	mHWnd = window->GetHWnd();
	HRESULT hr = DirectInput8Create(
		window->GetHInst(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		reinterpret_cast<void**>(mDInput.ReleaseAndGetAddressOf()), nullptr);
	MY_ASSERT(SUCCEEDED(hr));

	// ==================================================
	// Keyboard
	// ==================================================
	hr = mDInput->CreateDevice(GUID_SysKeyboard, &mKeyboardDev, nullptr);
	MY_ASSERT(SUCCEEDED(hr));
	hr = mKeyboardDev->SetDataFormat(&c_dfDIKeyboard);
	MY_ASSERT(SUCCEEDED(hr));
	hr = mKeyboardDev->SetCooperativeLevel(mHWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	MY_ASSERT(SUCCEEDED(hr));

	// ==================================================
	// Mouse
	// ==================================================
	hr = mDInput->CreateDevice(GUID_SysMouse, &mMouseDev, nullptr);
	MY_ASSERT(SUCCEEDED(hr));
	hr = mMouseDev->SetDataFormat(&c_dfDIMouse2);
	MY_ASSERT(SUCCEEDED(hr));
	hr = mMouseDev->SetCooperativeLevel(mHWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	MY_ASSERT(SUCCEEDED(hr));

	mKeyboardDev->Acquire();
	mMouseDev->Acquire();
}

void InputSystem::Terminate()
{
	mMouseDev->Unacquire();
	mKeyboardDev->Unacquire();
}

void InputSystem::Update()
{
	// ==================================================
	// Keyboard
	// ==================================================
	Keyboard& keyboard = mState.mKeyboard;
	memcpy(keyboard.mPrev, keyboard.mCurr, sizeof(keyboard.mCurr));
	ZeroMemory(keyboard.mCurr, sizeof(keyboard.mCurr));
	HRESULT hr = mKeyboardDev->GetDeviceState(sizeof(keyboard.mCurr), keyboard.mCurr);
	if (FAILED(hr))
	{
		mKeyboardDev->Acquire();
		mKeyboardDev->GetDeviceState(sizeof(keyboard.mCurr), keyboard.mCurr);
	}

	// ==================================================
	// Mouse
	// ==================================================
	Mouse& mouse = mState.mMouse;
	mouse.mPrev = mouse.mCurr;
	hr = mMouseDev->GetDeviceState(sizeof(mouse.mCurr), &mouse.mCurr);
	if (FAILED(hr))
	{
		mMouseDev->Acquire();
		mMouseDev->GetDeviceState(sizeof(mouse.mCurr), &mouse.mCurr);
	}
	POINT mousePosition = {};
	GetCursorPos(&mousePosition);
	ScreenToClient(mHWnd, &mousePosition);
	mouse.mPosition.x = float(mousePosition.x);
	mouse.mPosition.y = float(mousePosition.y);

	// ==================================================
	// Gamepad
	// ==================================================
	mState.mGamepad.mPrev = mState.mGamepad.mCurr;
	XInputGetState(0, &mState.mGamepad.mCurr);
}
