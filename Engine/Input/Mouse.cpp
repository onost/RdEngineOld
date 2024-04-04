#include "Mouse.h"
#include "Helper/MyAssert.h"

Mouse::Mouse()
	: mCurr()
	, mPrev()
	, mPosition(Vector2::kZero)
{}

bool Mouse::GetButton(uint8_t button) const
{
	MY_ASSERT(button >= 0 && button < _countof(mCurr.rgbButtons));
	return mCurr.rgbButtons[button] & 0x80;
}

bool Mouse::GetButtonUp(uint8_t button) const
{
	MY_ASSERT(button >= 0 && button < _countof(mCurr.rgbButtons));
	return !(mCurr.rgbButtons[button] & 0x80) && mPrev.rgbButtons[button] & 0x80;
}

bool Mouse::GetButtonDown(uint8_t button) const
{
	MY_ASSERT(button >= 0 && button < _countof(mCurr.rgbButtons));
	return mCurr.rgbButtons[button] & 0x80 && !(mPrev.rgbButtons[button] & 0x80);
}

Vector2 Mouse::GetMove() const
{
	return Vector2(
		static_cast<float>(mCurr.lX),
		static_cast<float>(mCurr.lY));
}

float Mouse::GetWheel() const
{
	return static_cast<float>(mCurr.lZ);
}
