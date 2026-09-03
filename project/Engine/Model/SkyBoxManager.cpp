#include "SkyBoxManager.h"

// Engine
#include <PipelineStateManager.h>
#include <CommandManager.h>
#include <ImguiWrapper.h>

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

void Cygnus::SkyBoxManager::Debug() {
#ifdef USE_IMGUI
	ImGui::Begin("SkyBoxManager");

	ImGui::DragFloat3("translate", &objectSkybox_->transform_.translate_.x, 0.01f);
	ImGui::DragFloat3("rotate", &objectSkybox_->transform_.rotate_.x, 0.01f);

	ImGui::Separator();

	ImGui::DragFloat4("color", &objectSkybox_->materialCB_.data_->color.x, 0.01f);
	ImGui::DragInt("enableLighting", &objectSkybox_->materialCB_.data_->enableLighting);
	ImGui::DragInt("useEnvironmentMap", &objectSkybox_->materialCB_.data_->useEnvironmentMap);
	ImGui::DragFloat("shininess", &objectSkybox_->materialCB_.data_->shininess, 0.01f);
	ImGui::DragFloat("environmentStrength", &objectSkybox_->materialCB_.data_->environmentStrength, 0.01f);
	ImGui::DragFloat3("emissiveColor", &objectSkybox_->materialCB_.data_->emissiveColor.x, 0.01f);
	ImGui::DragFloat("emissiveIntensity", &objectSkybox_->materialCB_.data_->emissiveIntensity, 0.01f);

	ImGui::End();
#endif
}

uint32_t Cygnus::SkyBoxManager::GetEnvironmentTextureHandle() { return modelSkybox_.material.textureHandle; }
