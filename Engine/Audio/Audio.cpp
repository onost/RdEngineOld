#include "Audio.h"
#include "Helper/MyAssert.h"
#include <fstream>

#pragma comment(lib, "xaudio2.lib")

void Audio::Initialize()
{
	HRESULT hr = XAudio2Create(&mXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	MyAssert(SUCCEEDED(hr));
	hr = mXAudio2->CreateMasteringVoice(&mMasterVoice);
	MyAssert(SUCCEEDED(hr));
}

void Audio::Terminate()
{
	mXAudio2.Reset();

	for (auto& sound : mSounds.GetResources())
	{
		delete[] sound.second->mBuff;
	}
}

Audio::SoundData* Audio::Load(const std::string& filePath)
{
	SoundData* soundData = mSounds.Get(filePath);
	if (soundData)
	{
		return soundData;
	}

	std::ifstream file;
	file.open(filePath, std::ios_base::binary);
	if (file.fail())
	{
		MyAssert(false);
		//return nullptr;
	}

	// RIFF
	RIFF riff = {};
	file.read((char*)&riff, sizeof(RIFF));
	if (strncmp(riff.mChunk.mID, "RIFF", 4) != 0)
	{
		MyAssert(false);
	}
	if (strncmp(riff.mFormat, "WAVE", 4) != 0)
	{
		MyAssert(false);
	}

	Chunk chunk = {};
	WAVEFORMATEX fmt = {};
	SoundData* result = new SoundData();
	while (!file.eof())
	{
		file.read((char*)&chunk, sizeof(Chunk));
		// fmt
		if (strncmp(chunk.mID, "fmt ", 4) == 0)
		{
			MyAssert(chunk.mSize <= sizeof(fmt));
			file.read((char*)&fmt, chunk.mSize);
			//
			//file.seekg(chunk.mSize - sizeof(WAVEFORMATEX), std::ios_base::cur);
		}
		// JUNK
		if (strncmp(chunk.mID, "JUNK", 4) == 0)
		{
			file.seekg(chunk.mSize, std::ios_base::cur);
		}
		// Data
		if (strncmp(chunk.mID, "data", 4) == 0)
		{
			char* buff = new char[chunk.mSize];
			file.read(buff, chunk.mSize);

			result->mFormat = fmt;
			result->mBuff = reinterpret_cast<BYTE*>(buff);
			result->mBuffSize = chunk.mSize;

			break;
		}
	}

	/*
	// fmt
	Fmt fmt = {};
	file.read((char*)&fmt, sizeof(Chunk));
	if (strncmp(fmt.mChunk.mID, "fmt", 4) != 0)
	{
		MyAssert(false);
	}
	MyAssert(fmt.mChunk.mSize <= sizeof(fmt.mFormat));
	file.read((char*)&fmt.mFormat, fmt.mChunk.mSize);

	// Data
	Chunk data = {};
	file.read((char*)&data, sizeof(Chunk));
	// ダミーデータ
	if (strncmp(data.mID, "JUNK", 4) != 0)
	{
		file.seekg(data.mSize, std::ios_base::cur);
		file.read((char*)&data, sizeof(Chunk));
	}
	if (strncmp(data.mID, "data", 4) != 0)
	{
		MyAssert(false);
	}
	char* buff = new char[data.mSize];
	file.read(buff, data.mSize);
	*/

	file.close();

	/*SoundData result = {};
	result.mFormat = fmt.mFormat;
	result.mBuff = reinterpret_cast<BYTE*>(buff);
	result.mBuffSize = data.mSize;*/
	mSounds.Add(filePath, result);
	return result;
}

void Audio::Play(SoundData* soundData)
{
	// SourceVoiceを作成
	IXAudio2SourceVoice* sourceVoice = nullptr;
	HRESULT hr = mXAudio2->CreateSourceVoice(&sourceVoice, &soundData->mFormat);
	MyAssert(SUCCEEDED(hr));

	// 
	XAUDIO2_BUFFER buff = {};
	buff.pAudioData = soundData->mBuff;
	buff.AudioBytes = soundData->mBuffSize;
	buff.Flags = XAUDIO2_END_OF_STREAM;

	// 再生
	hr = sourceVoice->SubmitSourceBuffer(&buff);
	hr = sourceVoice->Start();
}
