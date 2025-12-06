#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include "AbstractSceneFactory.h"
#include "BaseScene.h"

namespace Cygnus {
// =========================================================
// シーン管理クラス
// =========================================================
class SceneManager {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static SceneManager* GetInstance();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SceneManager();

	/// <summary>
	/// 指定したシーンに変更します。
	/// </summary>
	/// <param name="sceneName">シーン名</param>
	void ChangeScene(const std::string& sceneName);

	/// <summary>
	/// 毎フレーム更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// シーンファクトリーを設定します。
	/// </summary>
	/// <param name="sceneFactory">シーンファクトリー</param>
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

private:
	// =========================================================
	// Member Variables
	// =========================================================

	std::unique_ptr<BaseScene> scene_ = nullptr;			/* 現在のシーン */
	std::unique_ptr<BaseScene> nextScene_ = nullptr;		/* 次のシーン */
	AbstractSceneFactory* sceneFactory_ = nullptr;			/* シーンファクトリー */
};
}