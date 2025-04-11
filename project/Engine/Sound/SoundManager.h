#pragma once
#include <fstream>
#include <xaudio2.h>
#include <wrl.h>

#pragma comment(lib, "xaudio2.lib")

class SoundManager
{
public:
	// チャンクヘッダ
	struct ChunkHeader {
		char id[4]; // チャンク毎のID
		int32_t size; // チャンクサイズ
	};

	// RIFFヘッダチャンク
	struct RiffHeader {
		ChunkHeader chunk; // "RIFF"
		char type[4]; // "WAVE"
	};

	// FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk; // "fmt"
		WAVEFORMATEX fmt; // 波形フォーマット
	};

	// 音声データ
	struct SoundData {
		// 波形フォーマット
		WAVEFORMATEX wfex;
		// バッファの先頭アドレス
		BYTE* pBuffer;
		// バッファのサイズ
		unsigned int bufferSize;
	};

public:
	static SoundManager* GetInstance();

	~SoundManager();

	void Initialize();

	SoundData LoadWave(const char* filename);

	void Unload(SoundData* soundData);

	void PlayWave(const SoundData& soundData);

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
};

