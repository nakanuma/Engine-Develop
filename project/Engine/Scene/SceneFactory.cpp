#include "SceneFactory.h"

// TopDown-Shooting
//#include "TestScene/TestScene.h"
//#include "src/Game/Scene/TitleScene.h"
//#include "src/Game/Scene/GamePlayScene.h"

// CG5
#include "TestScene.h"
#include "TitleScene.h"
#include "GamePlayScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
    // 次のシーンを生成
    BaseScene* newScene = nullptr;

    if (sceneName == "TEST") {
		newScene = new TestScene();
		newScene->Initialize();
    } else if (sceneName == "TITLE") {
        newScene = new TitleScene();
        newScene->Initialize();
    } else if (sceneName == "GAMEPLAY") {
        newScene = new GamePlayScene();
        newScene->Initialize();
    }

    return newScene;
}
