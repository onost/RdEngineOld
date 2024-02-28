#include "Audio.h"
#include "Helper/MyAssert.h"

#pragma comment(lib, "xaudio2.lib")

void Audio::Initialize()
{
	HRESULT hr = XAudio2Create(&mXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	MyAssert(SUCCEEDED(hr));
	hr = mXAudio2->CreateMasteringVoice(&mMasterVoice);
	MyAssert(SUCCEEDED(hr));
}
