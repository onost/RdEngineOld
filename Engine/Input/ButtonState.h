#pragma once

// ボタンの状態
enum class ButtonState
{
	kNone,		// 押していない
	kPressed,	// 押した
	kRelease,	// 離した
	kHeld		// 押している
};
