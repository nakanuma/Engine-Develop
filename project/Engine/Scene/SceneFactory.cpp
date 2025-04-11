#include "SceneFactory.h"
#include "TitleScene.h"
#include "GamePlayScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
    // 次のシーンを生成
    BaseScene* newScene = nullptr;

    if (sceneName == "TITLE") {
        newScene = new TitleScene();
        newScene->Initialize();
    } else if (sceneName == "GAMEPLAY") {
        newScene = new GamePlayScene();
        newScene->Initialize();
    }

    return newScene;
}
