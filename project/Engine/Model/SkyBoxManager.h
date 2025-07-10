#pragma once
#include "Object3D.h"

/// <summary>
/// スカイボックス管理
/// </summary>
class SkyBoxManager
{
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static SkyBoxManager* GetInstance();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="filePath"></param>
	void Initialize(const std::string& filePath);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理（必ず環境マップ適用モデル描画前に描画）
	/// </summary>
	void Draw();

	/// <summary>
	/// 使用テクスチャの取得
	/// </summary>
	/// <returns></returns>
	uint32_t GetEnvironmentTextureHandle();

private:
	// モデルデータ（Skybox）
	ModelManager::ModelData modelSkybox_;
	// 3Dオブジェクト（Skybox）
	std::unique_ptr<Object3D> objectSkybox_;
};
