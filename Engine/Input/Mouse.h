#pragma once
#include "Vector2.h"
#include <dinput.h>

// マウス
class Mouse
{
	friend class InputSystem;

public:
	Mouse();

	bool GetButton(uint8_t button) const;
	bool GetButtonUp(uint8_t button) const;
	bool GetButtonDown(uint8_t button) const;
	//ButtonState GetState(uint8_t keyCode) const;

	const Vector2& GetPosition() const { return mPosition; }
	Vector2 GetMove() const;
	float GetWheel() const;

private:
	DIMOUSESTATE2 mCurr;
	DIMOUSESTATE2 mPrev;
	Vector2 mPosition;
};
