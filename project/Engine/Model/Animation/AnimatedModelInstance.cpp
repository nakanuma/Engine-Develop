#include "AnimatedModelInstance.h"

// Engine
#include <TextureManager.h>
#include <PipelineStateManager.h>
#include <CommandManager.h>

void Cygnus::AnimatedModelInstance::Initialize(const AnimatedModelData& data) {
	object_ = std::make_unique<Object3D>();

	data_.modelData = data.modelData;
	data_.animation = data.animation;
	data_.skeleton = data.skeleton;

	// モデルのセット
	object_->model_ = &data_.modelData;

	data_.skinCluster = SkinCluster();
	data_.skinCluster.CreateSkinCluster(DirectXBase::GetInstance()->GetDevice(), data_.skeleton, data_.modelData);
}

void Cygnus::AnimatedModelInstance::SetData(const AnimatedModelData& data) {
	data_.animation = data.animation;

	// モデルのセット
	object_->model_ = &data_.modelData;
}

void Cygnus::AnimatedModelInstance::Update(float deltaTime, bool isPlaying) {
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

void Cygnus::AnimatedModelInstance::Draw() {
	auto cmd = CommandManager::GetInstance()->GetCommandList();

	// Skinning用PSOに変更
	cmd->SetPipelineState(PipelineStateManager::GetInstance()->GetPSO(PSOType::Skinning));
	object_->Draw(data_.skinCluster);
	cmd->SetPipelineState(PipelineStateManager::GetInstance()->GetPSO(PSOType::Default));
}

void Cygnus::AnimatedModelInstance::DrawShadow() {
	auto cmd = CommandManager::GetInstance()->GetCommandList();

	// 骨行列SRVのバインド
	cmd->SetGraphicsRootDescriptorTable(5, data_.skinCluster.paletteSrvHandle_.second);

	object_->DrawShadow(data_.skinCluster);
}

void Cygnus::AnimatedModelInstance::SetAnimation(const AnimationLoader::Animation& animation) {
	data_.animation = animation;
	animationTime_ = 0.0f;
}
