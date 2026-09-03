#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include "Object3D.h"

namespace Cygnus {
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

	/// <summary>
	/// デバッグ表示を行います。
	/// </summary>
	void Debug();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// 使用テクスチャを取得します。
	/// </summary>
	/// <returns>テクスチャハンドル</returns>
	uint32_t GetEnvironmentTextureHandle();

	/// <summary>
	/// 位置を設定します。
	/// </summary>
	/// <param name="translate"></param>
	void SetTranslate(const Cygnus::Float3& translate) { objectSkybox_->transform_.translate_ = translate; }

	/// <summary>
	/// 回転を設定します。
	/// </summary>
	/// <param name="rotate"></param>
	void SetRotate(const Cygnus::Float3& rotate) { objectSkybox_->transform_.rotate_ = rotate; }

	/// <summary>
	/// 色を設定します。
	/// </summary>
	/// <param name="color"></param>
	void SetColor(const Cygnus::Float4& color) { objectSkybox_->materialCB_.data_->color = color; }

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kSkyBoxScale = 1000.0f;				/* スカイボックスのスケール */
	static constexpr uint32_t kRootParameterIndexCubeMap = 8;	/* キューブマップテクスチャ用ルートパラメーターインデックス */

	// =========================================================
	// Member Variables
	// =========================================================
	ModelManager::ModelData modelSkybox_;			/* スカイボックスモデルデータ */
	std::unique_ptr<Object3D> objectSkybox_;		/* スカイボックスオブジェクト */
};
}