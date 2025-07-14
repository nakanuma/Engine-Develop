#include "PostEffectManager.h"

// Engine
#include <RTVManager.h>
#include <Sprite.h>

void PostEffectManager::Initialize()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// レンダーテクスチャ作成
	renderTextureHandle_ = RTVManager::CreateRenderTargetTexture(Window::GetWidth(), Window::GetHeight());

	// 頂点バッファ
	vertexBuffer_ = CreateBufferResource(dxBase->GetDevice(), sizeof(Sprite::VertexData) * 4);
	vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeof(Sprite::VertexData) * 4;
	vbView_.StrideInBytes = sizeof(Sprite::VertexData);

	Sprite::VertexData* vbData = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vbData));
	vbData[0] = { {-1, -1, 0, 1}, {0, 1}, {0, 0, -1} };
	vbData[1] = { {-1, 1, 0, 1}, {0, 0}, {0, 0, -1} };
	vbData[2] = { {1, -1, 0, 1}, {1, 1}, {0, 0, -1} };
	vbData[3] = { {1, 1, 0, 1}, {1, 0}, {0, 0, -1} };

	// インデックスバッファ
	indexBuffer_ = CreateBufferResource(dxBase->GetDevice(), sizeof(uint32_t) * 6);
	ibView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	ibView_.SizeInBytes = sizeof(uint32_t) * 6;
	ibView_.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* ibData = nullptr;
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&ibData));
	ibData[0] = 0; ibData[1] = 1; ibData[2] = 2;
	ibData[3] = 1; ibData[4] = 3; ibData[5] = 2;

	// Transformation CB
	transformCB_ = CreateBufferResource(dxBase->GetDevice(), sizeof(Object3D::TransformationMatrix));
	transformCB_->Map(0, nullptr, reinterpret_cast<void**>(&transformMap_));
	transformMap_->WVP = Matrix::Identity();

	// Material CB
	materialCB_ = CreateBufferResource(dxBase->GetDevice(), sizeof(Object3D::Material));
	materialCB_->Map(0, nullptr, reinterpret_cast<void**>(&materialMap_));
	materialMap_->color = { 1, 1, 1, 1 };
	materialMap_->enableLighting = false;
	materialMap_->uvTransform = Matrix::Identity();
}

void PostEffectManager::BeginRenderToTexture()
{
	RTVManager::SetRenderTarget(renderTextureHandle_);
	RTVManager::ClearRTV(renderTextureHandle_);
}

void PostEffectManager::ApplyEffect()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();
	auto cmd = dxBase->GetCommandList();

	RTVManager::SetRTtoBB();

	switch (effectType_) {
	case PostEffectType::RadialBlur:
		cmd->SetPipelineState(dxBase->GetPipelineStateRadialBlur());
		break;

	case PostEffectType::GrayScale:
		cmd->SetPipelineState(dxBase->GetPipelineStateGrayscale());
		break;

	case PostEffectType::Vignette:
		cmd->SetPipelineState(dxBase->GetPipelineStateVignette());
		break;

	case PostEffectType::BoxFilter:
		cmd->SetPipelineState(dxBase->GetPipelineStateBoxFilter());
		break;

	case PostEffectType::GaussianFilter:
		cmd->SetPipelineState(dxBase->GetPipelineStateGaussianFilter());
		break;
	}

	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);

	cmd->SetGraphicsRootConstantBufferView(1, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(2, cmd, renderTextureHandle_);

	cmd->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
