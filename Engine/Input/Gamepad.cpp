#include "Gamepad.h"

bool Gamepad::GetButton(int button) const
{
	return mCurr.Gamepad.wButtons & button;
}

bool Gamepad::GetButtonUp(int button) const
{
	return
		!(mCurr.Gamepad.wButtons & button) &&
		mPrev.Gamepad.wButtons & button;
}

bool Gamepad::GetButtonDown(int button) const
{
	return
		mCurr.Gamepad.wButtons & button &&
		!(mPrev.Gamepad.wButtons & button);
}

Vector2 Gamepad::GetLStick() const
{
	return Vector2(
		mCurr.Gamepad.sThumbLX / 32767.0f,
		mCurr.Gamepad.sThumbLY / 32767.0f);
}

Vector2 Gamepad::GetRStick() const
{
	return Vector2(
		mCurr.Gamepad.sThumbRX / 32767.0f,
		mCurr.Gamepad.sThumbRY / 32767.0f);
}

float Gamepad::GetLTrigger() const
{
	return mCurr.Gamepad.bLeftTrigger / 255.0f;
}

float Gamepad::GetRTrigger() const
{
	return mCurr.Gamepad.bRightTrigger / 255.0f;
}
