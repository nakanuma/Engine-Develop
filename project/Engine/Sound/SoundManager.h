#pragma once
#include <fstream>
#include <wrl.h>
#include <xaudio2.h>

#pragma comment(lib, "xaudio2.lib")

/// <summary>
/// サウンド管理クラス
/// </summary>
class SoundManager {
public:
	/// <summary>
	/// WAV ファイル内のチャンクヘッダ構造体
	/// </summary>
	struct ChunkHeader {
		char id[4];   // チャンク毎のID
		int32_t size; // チャンクサイズ
	};

	/// <summary>
	/// RIFF ヘッダチャンク
	/// </summary>
	struct RiffHeader {
		ChunkHeader chunk; // "RIFF"
		char type[4];      // "WAVE"
	};

	/// <summary>
	/// フォーマットチャンク
	/// </summary>
	struct FormatChunk {
		ChunkHeader chunk; // "fmt"
		WAVEFORMATEX fmt;  // 波形フォーマット
	};

	/// <summary>
	/// 音声データ構造体
	/// </summary>
	struct SoundData {
		// 波形フォーマット
		WAVEFORMATEX wfex;
		// バッファの先頭アドレス
		BYTE* pBuffer;
		// バッファのサイズ
		unsigned int bufferSize;
		// 再生に使用するSourceVoice
		IXAudio2SourceVoice* pSourceVoice;
	};

public:
	static SoundManager* GetInstance();

	~SoundManager();

	void Initialize();

	SoundData LoadWave(const char* filename);

	void Unload(SoundData* soundData);

	void PlayWave(SoundData& soundData, bool loopFlag = false, float volume = 1.0f);

	void StopWave(SoundData& soundData);

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
};
