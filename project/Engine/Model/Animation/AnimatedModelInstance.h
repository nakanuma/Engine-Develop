#pragma once

// C++
#include <string>

// Engine
#include <DirectXBase.h>
#include <Object3D.h>
#include <ModelManager.h>
#include <Model/Animation/AnimationLoader.h>
#include <Model/Animation/Skeleton.h>
#include <Model/Animation/SkinCluster.h>

/// <summary>
/// アニメーションするモデルを一括管理
/// </summary>
class AnimatedModelInstance {
public:
	/// <summary>
	/// Animationモデルのロード
	/// </summary>
	bool Load(const std::string& directory, const std::string& filename);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float deltaTime);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();


	// モデルデータ
	ModelManager::ModelData modelData_;
	// オブジェクト
	std::unique_ptr<Object3D> object_;
	// アニメーション
	AnimationLoader::Animation animation_;
	// スケルトン
	Skeleton skeleton_;
	// スキンクラスター
	SkinCluster skinCluster_;
	// アニメーション時間
	float animationTime_ = 0.0f;
};
