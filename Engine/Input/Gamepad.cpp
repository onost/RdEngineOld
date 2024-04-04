#include "Gamepad.h"

Gamepad::Gamepad()
	: mCurr()
	, mPrev()
{}

bool Gamepad::GetButton(uint16_t button) const
{
	return mCurr.Gamepad.wButtons & button;
}

bool Gamepad::GetButtonUp(uint16_t button) const
{
	return !(mCurr.Gamepad.wButtons & button) && mPrev.Gamepad.wButtons & button;
}

bool Gamepad::GetButtonDown(uint16_t button) const
{
	return mCurr.Gamepad.wButtons & button && !(mPrev.Gamepad.wButtons & button);
}

Vector2 Gamepad::GetLeftStick() const
{
	return Vector2(
		mCurr.Gamepad.sThumbLX / 32767.0f,
		mCurr.Gamepad.sThumbLY / 32767.0f);
}

Vector2 Gamepad::GetRightStick() const
{
	return Vector2(
		mCurr.Gamepad.sThumbRX / 32767.0f,
		mCurr.Gamepad.sThumbRY / 32767.0f);
}

float Gamepad::GetLeftTrigger() const
{
	return mCurr.Gamepad.bLeftTrigger / 255.0f;
}

float Gamepad::GetRightTrigger() const
{
	return mCurr.Gamepad.bRightTrigger / 255.0f;
}
