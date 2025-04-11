#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"

// シーン管理
class SceneManager
{
public:
	static SceneManager* GetInstance();

	~SceneManager();

	// 次シーン予約
	/*void SetNextScene(BaseScene* nextScene) { nextScene_ = nextScene; }*/
	void ChangeScene(const std::string& sceneName);

	// 更新
	void Update();

	// 描画
	void Draw();

	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

private:
	// 今のシーン（実行中シーン）
	BaseScene* scene_ = nullptr;
	// 次のシーン
	BaseScene* nextScene_ = nullptr;
	// シーンファクトリー（借りてくる）
	AbstractSceneFactory* sceneFactory_ = nullptr;
};

