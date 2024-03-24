#pragma once
#include <cstdint>

// キーボード
class Keyboard
{
	friend class InputSystem;
public:
	bool GetKey(uint8_t keyCode) const;
	bool GetKeyUp(uint8_t keyCode) const;
	bool GetKeyDown(uint8_t keyCode) const;
private:
	uint8_t mCurr[256];
	uint8_t mPrev[256];
};
