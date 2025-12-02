#include "MyGame.h"
#include "SceneFactory.h"
#include "../Application/src/Game/GameResourceLoader/GameResourceLoader.h"
#include <StringUtil.h>

void MyGame::Initialize() {
	///
	/// 基底クラスの初期化処理
	///

	Framework::Initialize();

	///
	/// ゲーム固有の初期化
	///

	// ゲームリーソスローダーの初期化
	GameResourceLoader::GetInstance()->Initialize();

	// シーンファクトリーを生成し、マネージャにセット
	sceneFactory_ = SceneFactory::GetInstance();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);
	// シーンマネージャに最初のシーンをセット
	std::string initialSceneName = sceneFactory_->GetInitialSceneName();
	SceneManager::GetInstance()->ChangeScene(initialSceneName);
}

void MyGame::Finalize() {
	///
	/// ゲーム固有の終了処理
	///

	///
	/// 基底クラスの終了処理
	///

	Framework::Finalize();
}

void MyGame::Update() {
	///
	/// 基底クラスの更新処理
	///

	Framework::Update();

	///
	/// ゲーム固有の更新処理
	///
}

void MyGame::Draw() {
	// シーンの描画処理
	SceneManager::GetInstance()->Draw();
}
