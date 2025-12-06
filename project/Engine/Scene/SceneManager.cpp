#include "SceneManager.h"
#include <cassert>

Cygnus::SceneManager* Cygnus::SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

Cygnus::SceneManager::~SceneManager() {
	// 最後のシーンの終了と開放
	if (scene_) {
		scene_->Finalize();
	}
}

void Cygnus::SceneManager::ChangeScene(const std::string& sceneName) {
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	// 次シーンを生成
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}

void Cygnus::SceneManager::Update() {
	///
	///	シーン切り替え機構
	///

	// 次シーンの予約があるなら
	if (nextScene_) {
		// 旧シーンの終了
		if (scene_) {
			scene_->Finalize();
		}

		// シーン切り替え
		scene_ = std::move(nextScene_);
		// 次シーンの予約をクリア
		nextScene_ = nullptr;
	}

	// 実行中シーンを更新する
	if (scene_) {
		scene_->Update();
	}
}

void Cygnus::SceneManager::Draw() {
	// 実行中シーンの描画
	if (scene_) {
		scene_->Draw();
	}
}
