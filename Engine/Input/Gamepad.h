#pragma once
#include "Vector2.h"
#include <Windows.h>
#include <Xinput.h>

// ゲームパッド
class Gamepad
{
	friend class InputSystem;

public:
	Gamepad();

	bool GetButton(uint16_t button) const;
	bool GetButtonUp(uint16_t button) const;
	bool GetButtonDown(uint16_t button) const;
	//ButtonState GetState(uint8_t keyCode) const;

	// スティック
	Vector2 GetLeftStick() const;
	Vector2 GetRightStick() const;
	// トリガー
	float GetLeftTrigger() const;
	float GetRightTrigger() const;

private:
	XINPUT_STATE mCurr;
	XINPUT_STATE mPrev;
};
