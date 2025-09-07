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
	struct AnimatedModelData {
		// モデルデータ
		ModelManager::ModelData modelData;
		// アニメーション
		AnimationLoader::Animation animation;
		// スケルトン
		Skeleton skeleton;
		// スキンクラスター
		SkinCluster skinCluster;
	};

	/// <summary>
	/// アニメーションデータのセット
	/// </summary>
	/// <param name="data"></param>
	void SetData(const AnimatedModelData& data);

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


	// オブジェクト
	std::unique_ptr<Object3D> object_;
	// アニメーション用データ
	AnimatedModelData data_;

	// アニメーション時間
	float animationTime_ = 0.0f;
	// 再生速度
	float playbackSpeed_ = 1.0f;
	// ループするかどうか
	bool loop_ = true;
};
