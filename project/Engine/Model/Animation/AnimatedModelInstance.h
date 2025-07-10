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
	void Update(float deltaTime, bool isPlaying);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// アニメーションのセット
	/// </summary>
	void SetAnimation(const AnimationLoader::Animation& animation);

	/// <summary>
	/// ループするかの設定
	/// </summary>
	void SetLoop(bool loop) { loop_ = loop; }

	/// <summary>
	/// 再生速度の設定
	/// </summary>
	void SetPlayBackSpeed(float speed) { playbackSpeed_ = speed; }


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
	// 再生速度
	float playbackSpeed_ = 1.0f;
	// ループするかどうか
	bool loop_ = true;
};
