#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include "Object3D.h"

// =========================================================
// スカイボックス管理クラス
// =========================================================
class SkyBoxManager {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static SkyBoxManager* GetInstance();

	/// <summary>
	/// スカイボックスの初期化処理を行います。
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	void Initialize(const std::string& filePath);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// スカイボックスの描画処理を行います。（必ず環境マップ適用モデル描画前に描画する）
	/// </summary>
	void Draw();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// 使用テクスチャを取得します。
	/// </summary>
	/// <returns>テクスチャハンドル</returns>
	uint32_t GetEnvironmentTextureHandle();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	ModelManager::ModelData modelSkybox_;			/* スカイボックスモデルデータ */
	std::unique_ptr<Object3D> objectSkybox_;		/* スカイボックスオブジェクト */
};
