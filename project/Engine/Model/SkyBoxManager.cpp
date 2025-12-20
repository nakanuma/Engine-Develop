#include "SkyBoxManager.h"

// Engine
#include <PipelineStateManager.h>
#include <CommandManager.h>

Cygnus::SkyBoxManager* Cygnus::SkyBoxManager::GetInstance() {
	static SkyBoxManager instance;
	return &instance;
}

void Cygnus::SkyBoxManager::Initialize(const std::string& filePath) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// ddsファイルの読み込み
	uint32_t dds = TextureManager::Load(filePath);

	// モデルの読み込みとテクスチャの設定
	modelSkybox_ = ModelManager::CreateSkyBoxModel(dxBase->GetDevice());
	modelSkybox_.material.textureHandle = dds;

	// オブジェクトの生成とモデル設定
	objectSkybox_ = std::make_unique<Object3D>();
	objectSkybox_->model_ = &modelSkybox_;
	objectSkybox_->transform_.scale_ = { kSkyBoxScale, kSkyBoxScale, kSkyBoxScale };
}

void Cygnus::SkyBoxManager::Update() {
	// Skybox更新
	objectSkybox_->UpdateMatrix();
}

void Cygnus::SkyBoxManager::Draw() {
	PipelineStateManager* psoManager = PipelineStateManager::GetInstance();
	auto cmd = CommandManager::GetInstance()->GetCommandList();

	// Skybox用PSOに変更
	cmd->SetPipelineState(psoManager->GetPSO(PSOType::Skybox));
	// CubeMapをバインド
	TextureManager::SetDescriptorTable(kRootParameterIndexCubeMap, cmd, modelSkybox_.material.textureHandle);
	// 描画
	objectSkybox_->Draw();
	// 通常PSOに戻す
	cmd->SetPipelineState(psoManager->GetPSO(PSOType::Default));
}

uint32_t Cygnus::SkyBoxManager::GetEnvironmentTextureHandle() { return modelSkybox_.material.textureHandle; }
