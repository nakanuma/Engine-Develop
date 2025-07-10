#include "AnimatedModelInstance.h"

// Engine
#include <TextureManager.h>

// ---------------------------------------------------------
// Animationモデルのロード
// ---------------------------------------------------------
bool AnimatedModelInstance::Load(const std::string& directory, const std::string& filename) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// モデル読み込み
	modelData_ = ModelManager::LoadModelFile(directory, filename, dxBase->GetDevice());
	modelData_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	// アニメーション読み込み
	animation_ = AnimationLoader::LoadAnimation(directory, filename);

	// スケルトン作成
	skeleton_.CreateSkeleton(modelData_.rootNode);

	// スキンクラスター作成
	skinCluster_.CreateSkinCluster(dxBase->GetDevice(), skeleton_, modelData_);

	// オブジェクト生成
	object_ = std::make_unique<Object3D>();
	object_->model_ = &modelData_;

	return true;
}

// ---------------------------------------------------------
// 更新
// ---------------------------------------------------------
void AnimatedModelInstance::Update(float deltaTime) {
	// アニメーション更新
	animationTime_ += deltaTime;
	animationTime_ = std::fmod(animationTime_, animation_.duration);

	// アニメーション -> スケルトン -> スキンクラスターの更新
	skeleton_.ApplyAnimation(animation_, animationTime_);
	skeleton_.Update();
	skinCluster_.Update(skeleton_);

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
	object_->Draw(skinCluster_);
	dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineState());
}
