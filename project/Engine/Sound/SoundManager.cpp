#include "SoundManager.h"
#include <cassert>

Cygnus::SoundManager* Cygnus::SoundManager::GetInstance() {
	static SoundManager instance;
	return &instance;
}

Cygnus::SoundManager::~SoundManager() { xAudio2_.Reset(); }

void Cygnus::SoundManager::Initialize() {
	HRESULT result;
	// メインインターフェースを生成
	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));
	// マスターボイスを生成
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(result));
}

void Cygnus::SoundManager::Load(const std::string& filename, const std::string& key) {
	// 既に読み込み済みならスキップ
	if (soundMap_.find(key) != soundMap_.end()) return;

	///
	/// 1, ファイルオープン
	///

	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	// ファイルオープン失敗を検出する
	assert(file.is_open());

	///
	/// 2, .wavデータ読み込み
	///

	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));

	// 不要なチャンクをスキップするループ
	while (strncmp(data.id, "data", 4) != 0) {
		file.seekg(data.size, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部（波形データ）の読み込み
	std::unique_ptr<BYTE[]> buffer = std::make_unique<BYTE[]>(data.size);
	file.read(reinterpret_cast<char*>(buffer.get()), data.size);

	///
	/// 3, ファイルクローズ
	///

	// Waveファイルを閉じる
	file.close();

	///
	/// 4, 読み込んだ音声データをマップに登録
	///

	// 音声データを作成し、解析したデータをセット
	auto soundData = std::make_unique<SoundData>();

	soundData->wfex = format.fmt;
	soundData->pBuffer = std::move(buffer);
	soundData->bufferSize = data.size;

	soundMap_[key] = std::move(soundData);
}

void Cygnus::SoundManager::Play(const std::string& key, bool loop, float volume) { 
	// 再生の終わった音をクリア
	ClearFinishedVoices();

	auto it = soundMap_.find(key); 
	if (it == soundMap_.end()) return; // 見つからなければスキップ

	SoundData& data = *(it->second);
	
	// 新しいソースボイスを作成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	HRESULT result = xAudio2_->CreateSourceVoice(&pSourceVoice, &data.wfex);
	assert(SUCCEEDED(result));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = data.pBuffer.get();
	buf.AudioBytes = data.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0; // ループ設定

	// 波形データの再生
	pSourceVoice->SubmitSourceBuffer(&buf);
	pSourceVoice->SetVolume(volume);
	pSourceVoice->Start();

	// キーとボイスをペアにして管理リストに追加
	PlayingVoice playingVoice;
	playingVoice.pSourceVoice = pSourceVoice;
	playingVoice.key = key;
	activeVoices_.push_back(playingVoice);
}

void Cygnus::SoundManager::Stop(const std::string& key) { 
	for (auto& voice : activeVoices_) {
		if (voice.key == key) {
			voice.pSourceVoice->Stop();
		}
	}
}

void Cygnus::SoundManager::ClearFinishedVoices() { 
	auto it = activeVoices_.begin(); 
	while (it != activeVoices_.end()) {
		XAUDIO2_VOICE_STATE state;
		it->pSourceVoice->GetState(&state);

		// 再生中のバッファが0になったら終了とみなす
		if (state.BuffersQueued == 0) {
			it->pSourceVoice->DestroyVoice();
			it = activeVoices_.erase(it); // リストから削除して次の要素へ
		} else {
			++it;
		}
	}
}
