#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <fstream>
#include <wrl.h>
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

// =========================================================
// サウンド管理クラス
// =========================================================
class SoundManager {
public:
	/// <summary>
	/// WAV ファイル内のチャンクヘッダ構造体
	/// </summary>
	struct ChunkHeader {
		char id[4];							/* チャンクID */
		int32_t size;						/* チャンクサイズ */
	};

	/// <summary>
	/// RIFF ヘッダチャンク
	/// </summary>
	struct RiffHeader {
		ChunkHeader chunk;					/* チャンクヘッダ */
		char type[4];						/* チャンクタイプ */
	};

	/// <summary>
	/// フォーマットチャンク
	/// </summary>
	struct FormatChunk {
		ChunkHeader chunk;					/* チャンクヘッダ */
		WAVEFORMATEX fmt;					/* 波形フォーマット */
	};

	/// <summary>
	/// 音声データ構造体
	/// </summary>
	struct SoundData {
		WAVEFORMATEX wfex;					/* 波形フォーマット */
		BYTE* pBuffer;						/* 音声バッファ */
		unsigned int bufferSize;			/* バッファサイズ */
		IXAudio2SourceVoice* pSourceVoice;	/* ソースボイス */
	};

public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static SoundManager* GetInstance();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SoundManager();

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	void Initialize();

	/// <summary>
	/// WAVファイルを読み込みます。
	/// </summary>
	/// <param name="filename">ファイル名</param>
	/// <returns>読み込んだ音声データ</returns>
	SoundData LoadWave(const char* filename);

	/// <summary>
	/// 音声データを解放します。
	/// </summary>
	/// <param name="soundData">音声データ</param>
	void Unload(SoundData* soundData);

	/// <summary>
	/// 音声データを再生します。
	/// </summary>
	/// <param name="soundData">音声データ</param>
	/// <param name="loopFlag">ループフラグ</param>
	/// <param name="volume">音量</param>
	void PlayWave(SoundData& soundData, bool loopFlag = false, float volume = 1.0f);

	/// <summary>
	/// 音声データの再生を停止します。
	/// </summary>
	/// <param name="soundData">音声データ</param>
	void StopWave(SoundData& soundData);

private:
	// =========================================================
	// Member Variables
	// =========================================================

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;			/* XAudio2 インターフェース */
	IXAudio2MasteringVoice* masterVoice;				/* マスターボイス */
};
