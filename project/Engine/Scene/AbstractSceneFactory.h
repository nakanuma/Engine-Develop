#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <string>

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
	virtual BaseScene* CreateScene(const std::string& sceneName) = 0;
};
