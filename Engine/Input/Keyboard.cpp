#include "Keyboard.h"
#include "Helper/MyAssert.h"

bool Keyboard::GetKey(uint8_t keyCode) const
{
	MY_ASSERT(keyCode >= 0 && keyCode < _countof(mCurr));
	return mCurr[keyCode] & 0x80;
}

bool Keyboard::GetKeyUp(uint8_t keyCode) const
{
	MY_ASSERT(keyCode >= 0 && keyCode < _countof(mCurr));
	return !(mCurr[keyCode] & 0x80) && mPrev[keyCode] & 0x80;
}

bool Keyboard::GetKeyDown(uint8_t keyCode) const
{
	MY_ASSERT(keyCode >= 0 && keyCode < _countof(mCurr));
	return mCurr[keyCode] & 0x80 && !(mPrev[keyCode] & 0x80);
}
