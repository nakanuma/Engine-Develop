#include "AnimatedModelInstance.h"

// Engine
#include <TextureManager.h>

// ---------------------------------------------------------
// アニメーションデータのセット
// ---------------------------------------------------------
void AnimatedModelInstance::SetData(const AnimatedModelData& data)
{
	data_.modelData = data.modelData;
	data_.animation = data.animation;
	data_.skeleton = data.skeleton;
	data_.skinCluster = data.skinCluster;

	// オブジェクト生成
	object_ = std::make_unique<Object3D>();
	object_->model_ = &data_.modelData;
}

// ---------------------------------------------------------
// 更新
// ---------------------------------------------------------
void AnimatedModelInstance::Update(float deltaTime, bool isPlaying) {
	// アニメーション更新
	if (isPlaying) {
		animationTime_ += deltaTime * playbackSpeed_;
	}
	// ループ再生
	if (loop_ && data_.animation.duration > 0.0f) {
		animationTime_ = std::fmod(animationTime_, data_.animation.duration);
	}

	// アニメーション -> スケルトン -> スキンクラスターの更新
	data_.skeleton.ApplyAnimation(data_.animation, animationTime_);
	data_.skeleton.Update();
	data_.skinCluster.Update(data_.skeleton);

	// オブジェクト更新
	object_->UpdateMatrix();
}

// ---------------------------------------------------------
// 描画
// ---------------------------------------------------------
void AnimatedModelInstance::Draw() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// Skinning用PSOに変更
	dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineStateSkinning());
	object_->Draw(data_.skinCluster);
	dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineState());
}

// ---------------------------------------------------------
// アニメーションのセット
// ---------------------------------------------------------
void AnimatedModelInstance::SetAnimation(const AnimationLoader::Animation& animation) { 
	data_.animation = animation;
	animationTime_ = 0.0f;
}
