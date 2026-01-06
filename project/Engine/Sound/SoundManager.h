#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <fstream>
#include <wrl.h>
#include <xaudio2.h>
#include <unordered_map>
#pragma comment(lib, "xaudio2.lib")

namespace Cygnus {
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
		std::unique_ptr<BYTE[]> pBuffer;    /* 音声バッファ */
		unsigned int bufferSize;			/* バッファサイズ */
	};

	struct PlayingVoice {
		IXAudio2SourceVoice* pSourceVoice;	/* 再生中のソースボイス */
		std::string key;					/* 鳴らしている音の識別用キー */
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
	/// 音声を読み込んで登録します。
	/// </summary>
	/// <param name="filename">ファイルパス</param>
	/// <param name="key">任意のキー</param>
	void Load(const std::string& filename, const std::string& key);

	/// <summary>
	/// キー指定で音声を再生します。
	/// </summary>
	/// <param name="key">再生する音声のキー</param>
	/// <param name="loop">ループ再生フラグ</param>
	/// <param name="volume">音量</param>
	void Play(const std::string& key, bool loop = false, float volume = 1.0f);

	/// <summary>
	/// キー指定で音声を停止します。
	/// </summary>
	/// <param name="key"></param>
	void Stop(const std::string& key);

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// 再生の終了したボイスをクリアします。
	/// </summary>
	void ClearFinishedVoices();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;			/* XAudio2 インターフェース */
	IXAudio2MasteringVoice* masterVoice_;				/* マスターボイス */

	std::unordered_map<std::string, std::unique_ptr<SoundData>> soundMap_; /* 音声データを全て保持するマップ */
	std::list<IXAudio2SourceVoice*> activeVoices_;                         /* 再生中ボイスのリスト */
};
}