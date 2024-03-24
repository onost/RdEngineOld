#pragma once
#include "Core/ResourceBank.h"
#include <string>
#include <wrl.h>
#include <xaudio2.h>

// https://www.youfit.co.jp/archives/1418
// https://qiita.com/okmonn/items/ab037ef11bf0e2ebbaca
// https://necotech.org/archives/657

class AudioSystem
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
	void Terminate();

	// 読み込み
	SoundData* Load(const std::string& filePath);
	// 再生
	void Play(SoundData* soundData);

private:
	Microsoft::WRL::ComPtr<IXAudio2> mXAudio2;
	IXAudio2MasteringVoice* mMasterVoice;
	// アセット
	ResourceBank<SoundData> mSounds;
};
