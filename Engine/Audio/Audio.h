#pragma once
#include <xaudio2.h>
#include <wrl.h>

// https://www.youfit.co.jp/archives/1418

class Audio
{
public:
	// チャンク
	struct Chunk
	{
		char mID[4];
		int32_t mSize;
	};

	// RIFF
	struct RIFF
	{
		Chunk mChunk;
		char mFormat[4];// WAVE,AVI
	};
	// fmt
	struct Fmt
	{
		Chunk mChunk;
		WAVEFORMATEX mFormat;
	};
	// Data
	/*struct Data
	{
		Chunk mChunk;
		//
	};*/

	struct SoundData
	{
		WAVEFORMATEX mFormat;
		BYTE* mBuff;// バッファの先頭アドレス
		uint32_t mBuffSize;// バッファ
	};

	void Initialize();

private:
	Microsoft::WRL::ComPtr<IXAudio2> mXAudio2;
	IXAudio2MasteringVoice* mMasterVoice;
};
