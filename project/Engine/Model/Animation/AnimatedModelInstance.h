#pragma once

// C++
#include <string>

// Engine
#include <DirectXBase.h>
#include <Model/Animation/AnimationLoader.h>
#include <Model/Animation/Skeleton.h>
#include <Model/Animation/SkinCluster.h>
#include <ModelManager.h>
#include <Object3D.h>

/// <summary>
/// アニメーション付きのモデルのインスタンス管理クラス
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
	/// 初期化処理
	/// </summary>
	void Initialize(const AnimatedModelData& data);

	/// <summary>
	/// アニメーションデータのセット
	/// </summary>
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
	/// 描画（シャドウマップ用）
	/// </summary>
	void DrawShadow();

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

	Float3& GetTranslate() { return object_->transform_.translate; }
	Float3& GetRotate() { return object_->transform_.rotate; }
	Float3& GetScale() { return object_->transform_.scale; }

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
