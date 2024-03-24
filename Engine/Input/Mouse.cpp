#include "Mouse.h"
#include "Helper/MyAssert.h"

bool Mouse::GetButton(uint8_t button) const
{
	MY_ASSERT(button >= 0 && button < _countof(mCurr.rgbButtons));
	return mCurr.rgbButtons[button] & 0x80;
}

bool Mouse::GetButtonUp(uint8_t button) const
{
	MY_ASSERT(button >= 0 && button < _countof(mCurr.rgbButtons));
	return mCurr.rgbButtons[button] & 0x80 && !(mPrev.rgbButtons[button] & 0x80);
}

bool Mouse::GetButtonDown(uint8_t button) const
{
	MY_ASSERT(button >= 0 && button < _countof(mCurr.rgbButtons));
	return !(mCurr.rgbButtons[button] & 0x80) && mPrev.rgbButtons[button] & 0x80;
}
