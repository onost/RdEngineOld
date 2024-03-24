#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include "Gamepad.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <dinput.h>
#include <wrl.h>

class Window;

class InputSystem
{
public:
	// 入力状況
	struct State
	{
		Keyboard mKeyboard;
		Mouse mMouse;
		Gamepad mGamepad;
	};

	void Initialize(Window* window);
	void Terminate();
	void Update();

	const State& GetState() const { return mState; }

private:
	HWND mHWnd;
	Microsoft::WRL::ComPtr<IDirectInput8> mDInput;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mKeyboardDev;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mMouseDev;
	State mState;
};
