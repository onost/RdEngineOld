#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include <dinput.h>

// マウス
class Mouse
{
	friend class InputSystem;
public:
	bool GetButton(uint8_t button) const;
	bool GetButtonUp(uint8_t button) const;
	bool GetButtonDown(uint8_t button) const;
	const Vector2& GetPosition() const { return mPosition; }
	Vector3 GetMove() const
	{
		return Vector3(
			float(mCurr.lX),
			float(mCurr.lY),
			float(mCurr.lZ));
	}
private:
	DIMOUSESTATE2 mCurr;
	DIMOUSESTATE2 mPrev;
	Vector2 mPosition;
};
