#pragma once
#include "Vector2.h"
#include <cstdint>
#include <Windows.h>
#include <Xinput.h>

// ゲームパッド
class Gamepad
{
	friend class InputSystem;
public:
	bool GetButton(int button) const;
	bool GetButtonUp(int button) const;
	bool GetButtonDown(int button) const;
	Vector2 GetLStick() const;
	Vector2 GetRStick() const;
	float GetLTrigger() const;
	float GetRTrigger() const;
private:
	XINPUT_STATE mCurr;
	XINPUT_STATE mPrev;
};
