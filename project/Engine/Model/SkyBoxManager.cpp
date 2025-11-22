#include "SkyBoxManager.h"

SkyBoxManager* SkyBoxManager::GetInstance() {
	static SkyBoxManager instance;
	return &instance;
}

void SkyBoxManager::Initialize(const std::string& filePath) {
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

void SkyBoxManager::Update() {
	// Skybox更新
	objectSkybox_->UpdateMatrix();
}

void SkyBoxManager::Draw() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// Skybox用PSOに変更 -> Skybox描画 -> 通常PSOに変更
	dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineStateSkybox());

	// CubeMapをバインド
	TextureManager::SetDescriptorTable(kRootParameterIndexCubeMap, dxBase->GetCommandList(), modelSkybox_.material.textureHandle);

	objectSkybox_->Draw();
	dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineState());
}

uint32_t SkyBoxManager::GetEnvironmentTextureHandle() { return modelSkybox_.material.textureHandle; }
