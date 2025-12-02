#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <string>
#include <memory>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <BaseScene.h>

// =========================================================
// シーン工場（概念）
// =========================================================
class AbstractSceneFactory {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~AbstractSceneFactory() = default;

	/// <summary>
	/// シーンの生成を行います。
	/// </summary>
	/// <param name="sceneName">シーン名</param>
	/// <returns>生成されたシーン</returns>
	virtual std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName) = 0;

	/// <summary>
	/// 初期化時のシーン名を取得します。
	/// </summary>
	/// <returns>初期化シーン名</returns>
	virtual std::string GetInitialSceneName() = 0;
};
