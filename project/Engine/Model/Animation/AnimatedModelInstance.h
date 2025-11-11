#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <string>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <DirectXBase.h>
#include <Model/Animation/AnimationLoader.h>
#include <Model/Animation/Skeleton.h>
#include <Model/Animation/SkinCluster.h>
#include <ModelManager.h>
#include <Object3D.h>

// =========================================================
// アニメーション付きのモデルのインスタンス管理クラス
// =========================================================
class AnimatedModelInstance {
public:
	/// <summary>
	/// アニメーション付きモデルのデータを保持する構造体
	/// </summary>
	struct AnimatedModelData {
		ModelManager::ModelData modelData;			/* モデルデータ */
		AnimationLoader::Animation animation;		/* アニメーションデータ */
		Skeleton skeleton;							/* スケルトン */
		SkinCluster skinCluster;					/* スキンクラスター */
	};

	// =========================================================
	// Public Methods
	// =========================================================


	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	/// <param name="data">アニメーションモデルデータ</param>
	void Initialize(const AnimatedModelData& data);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="deltaTime">デルタタイム</param>
	/// <param name="isPlaying">再生フラグ</param>
	void Update(float deltaTime, bool isPlaying);

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// シャドウマップ用の描画処理を行います。
	/// </summary>
	void DrawShadow();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// アニメーションデータをセットします。
	/// </summary>
	/// <param name="data">アニメーションモデルデータ</param>
	void SetData(const AnimatedModelData& data);

	/// <summary>
	/// アニメーションをセットします。
	/// </summary>
	/// <param name="animation">アニメーションデータ</param>
	void SetAnimation(const AnimationLoader::Animation& animation);

	/// <summary>
	/// ループするかどうかを設定します。
	/// </summary>
	/// <param name="loop">ループフラグ</param>
	void SetLoop(bool loop) { loop_ = loop; }

	/// <summary>
	/// 再生速度を設定します。
	/// </summary>
	/// <param name="speed">再生速度（1.0fなら等倍速）</param>
	void SetPlayBackSpeed(float speed) { playbackSpeed_ = speed; }

	/// <summary>
	/// 位置を取得します。
	/// </summary>
	/// <returns>現在位置（Float3）</returns>
	Float3& GetTranslate() { return object_->transform_.translate; }

	/// <summary>
	/// 回転を取得します。
	/// </summary>
	/// <returns>回転（Euler）</returns>
	Float3& GetRotate() { return object_->transform_.rotate; }

	/// <summary>
	/// スケールを取得します。
	/// </summary>
	/// <returns>スケール（Float3）</returns>
	Float3& GetScale() { return object_->transform_.scale; }

	// =========================================================
	// Member Variables
	// =========================================================

	std::unique_ptr<Object3D> object_;			/* オブジェクト */
	AnimatedModelData data_;					/* アニメーション付きモデルデータ */

	float animationTime_ = 0.0f;				/* アニメーションの現在時刻 */
	float playbackSpeed_ = 1.0f;				/* 再生速度 */
	bool loop_ = true;							/* ループフラグ */
};
