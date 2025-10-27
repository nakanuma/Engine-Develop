#include "PostEffectManager.h"

// Engine
#include <ParticleEffect/ParticleEffectManager.h>
#include <RTVManager.h>
#include <Sprite.h>

void PostEffectManager::Initialize() {
	// 既に初期化済みならスキップ
	if(initialized_) return;
	initialized_ = true;

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
	vbData[0] = {
	    {-1, -1, 0, 1},
        {0, 1},
        {0, 0, -1}
    };
	vbData[1] = {
	    {-1, 1, 0, 1},
        {0, 0},
        {0, 0, -1}
    };
	vbData[2] = {
	    {1, -1, 0, 1},
        {1, 1},
        {0, 0, -1}
    };
	vbData[3] = {
	    {1, 1, 0, 1},
        {1, 0},
        {0, 0, -1}
    };

	// インデックスバッファ
	indexBuffer_ = CreateBufferResource(dxBase->GetDevice(), sizeof(uint32_t) * 6);
	ibView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	ibView_.SizeInBytes = sizeof(uint32_t) * 6;
	ibView_.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* ibData = nullptr;
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&ibData));
	ibData[0] = 0;
	ibData[1] = 1;
	ibData[2] = 2;
	ibData[3] = 1;
	ibData[4] = 3;
	ibData[5] = 2;

	// Transformation CB
	transformCB_ = CreateBufferResource(dxBase->GetDevice(), sizeof(Object3D::TransformationMatrix));
	transformCB_->Map(0, nullptr, reinterpret_cast<void**>(&transformMap_));
	transformMap_->WVP = Matrix::Identity();

	// Material CB
	materialCB_ = CreateBufferResource(dxBase->GetDevice(), sizeof(Object3D::Material));
	materialCB_->Map(0, nullptr, reinterpret_cast<void**>(&materialMap_));
	materialMap_->color = {1.0f, 1.0f, 1.0f, 1.0f};
	materialMap_->enableLighting = false;
	materialMap_->uvTransform = Matrix::Identity();

	/*アウトライン*/

	// レンダーテクスチャ
	outlineRT_ = RTVManager::CreateRenderTargetTexture(Window::GetWidth(), Window::GetHeight(), {0.0f, 0.0f, 0.0f, 0.0f});
	outlineGH_ = RTVManager::CreateRenderTargetTexture(Window::GetWidth(), Window::GetHeight(), {0.0f, 0.0f, 0.0f, 0.0f});

	// Outline Material
	outlineMaterial_.data_->color = {0.0f, 0.0f, 0.0f, 1.0f};
	outlineMaterial_.data_->enableLighting = false;
	outlineMaterial_.data_->uvTransform = Matrix::Identity();

	/*Bloom*/
	bloomExtractGH_ = RTVManager::CreateRenderTargetTexture(Window::GetWidth(), Window::GetHeight(), {0.0f, 0.0f, 0.0f, 0.0f});
	bloomBlurGH_ = RTVManager::CreateRenderTargetTexture(Window::GetWidth(), Window::GetHeight(), {0.0f, 0.0f, 0.0f, 0.0f});

	/*WaveDistortion*/
	waveCB_.data_->gTime = 0.0f;
	waveCB_.data_->amplitude = 0.02f;
	waveCB_.data_->frequency = 10.0f;
	waveCB_.data_->speed = 1.5f;

	/*GlitchEffect*/
	glitchCB_.data_->gTime = 0.0f;
	glitchCB_.data_->intensity = 1.0f;
	glitchCB_.data_->speed = 0.5f;
}

void PostEffectManager::TransfarConstantBuffer() {
	/*WaveDistrotion*/
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(9, waveCB_.resource_->GetGPUVirtualAddress());
	/*GlitchEffect*/
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(10, glitchCB_.resource_->GetGPUVirtualAddress());
}

void PostEffectManager::BeginRenderToTexture() {
	if (effectType_ == PostEffectType::None) {
		return;
	}

	RTVManager::SetRenderTarget(renderTextureHandle_);
	RTVManager::ClearRTV(renderTextureHandle_);
}

void PostEffectManager::ApplyEffect() {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	auto cmd = dxBase->GetCommandList();

	RTVManager::SetRTtoBB();

	if (effectType_ == PostEffectType::None) {
		return;
	}

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

	case PostEffectType::InvertColor:
		cmd->SetPipelineState(dxBase->GetPipelineStateInvertColor());
		break;

	case PostEffectType::Sepia:
		cmd->SetPipelineState(dxBase->GetPipelineStateSepia());
		break;

	case PostEffectType::Posterize:
		cmd->SetPipelineState(dxBase->GetPipelineStatePosterize());
		break;

	case PostEffectType::Emboss:
		cmd->SetPipelineState(dxBase->GetPipelineStateEmboss());
		break;

	case PostEffectType::Sharpen:
		cmd->SetPipelineState(dxBase->GetPipelineStateSharpen());
		break;

	case PostEffectType::ColorAberration:
		cmd->SetPipelineState(dxBase->GetPipelineStateColorAberration());
		break;

	case PostEffectType::BarrelDistortion:
		cmd->SetPipelineState(dxBase->GetPipelineStateBarrelDistortion());
		break;

	case PostEffectType::WaveDistortion:
		cmd->SetPipelineState(dxBase->GetPipelineStateWaveDistortion());
		break;

	case PostEffectType::Pixelation:
		cmd->SetPipelineState(dxBase->GetPipelineStatePixelation());
		break;

	case PostEffectType::GlitchEffect:
		cmd->SetPipelineState(dxBase->GetPipelineStateGlitchEffect());
		break;
	}

	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);

	cmd->SetGraphicsRootConstantBufferView(1, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(2, cmd, renderTextureHandle_);

	cmd->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void PostEffectManager::BeginRenderToOutlineTexture() {
	RTVManager::SetRenderTarget(outlineRT_);
	RTVManager::ClearRTV(outlineRT_, {0.0f, 0.0f, 0.0f, 0.0f});
}

void PostEffectManager::ApplyOutline() {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	auto cmd = dxBase->GetCommandList();

#pragma region 深度値を元にアウトライン生成
	RTVManager::SetRenderTarget(outlineGH_);
	RTVManager::ClearRTV(outlineGH_, {0.0f, 0.0f, 0.0f, 0.0f});

	cmd->SetPipelineState(dxBase->GetPipelineStateSobelFilter());
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(0, outlineMaterial_.resource_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(1, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(2, cmd, RTVManager::GetDepthSRVHandle(outlineRT_));

	cmd->DrawIndexedInstanced(6, 1, 0, 0, 0);
#pragma endregion
}

void PostEffectManager::DrawOutline() {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	auto cmd = dxBase->GetCommandList();

#pragma region renderTextureGHをバックバッファにそのまま描画
	RTVManager::SetRTtoBB();

	cmd->SetPipelineState(dxBase->GetPipelineState());
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(0, materialCB_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(1, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(2, cmd, renderTextureHandle_);

	cmd->DrawIndexedInstanced(6, 1, 0, 0, 0);
#pragma endregion

#pragma region outlineをバックバッファにそのまま描画
	cmd->SetPipelineState(dxBase->GetPipelineState());
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(0, materialCB_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(1, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(2, cmd, outlineGH_);

	cmd->DrawIndexedInstanced(6, 1, 0, 0, 0);
#pragma endregion
}

void PostEffectManager::ApplyBloom() {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	auto cmd = dxBase->GetCommandList();

#pragma region 明るい部分の抽出
	RTVManager::SetRenderTarget(bloomExtractGH_);
	RTVManager::ClearRTV(bloomExtractGH_, {0.0f, 0.0f, 0.0f, 0.0f});

	cmd->SetPipelineState(dxBase->GetPipelineStateBloomExtract());
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(0, outlineMaterial_.resource_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(1, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(2, cmd, renderTextureHandle_);

	cmd->DrawIndexedInstanced(6, 1, 0, 0, 0);
#pragma endregion

	ParticleEffectManager::GetInstance()->Draw();

#pragma region ガウスブラー
	RTVManager::SetRenderTarget(bloomBlurGH_);
	RTVManager::ClearRTV(bloomBlurGH_, {0.0f, 0.0f, 0.0f, 0.0f});

	cmd->SetPipelineState(dxBase->GetPipelineStateGaussianFilter());
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(0, outlineMaterial_.resource_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(1, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(2, cmd, bloomExtractGH_); // 明るさ抽出結果にブラーをかける

	cmd->DrawIndexedInstanced(6, 1, 0, 0, 0);
#pragma endregion
}

void PostEffectManager::DrawBloom() {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	auto cmd = dxBase->GetCommandList();

#pragma region renderTextureGHをバックバッファに描画
	RTVManager::SetRTtoBB();

	cmd->SetPipelineState(dxBase->GetPipelineState());
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(0, materialCB_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(1, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(2, cmd, renderTextureHandle_);

	cmd->DrawIndexedInstanced(6, 1, 0, 0, 0);
#pragma endregion

#pragma region ブラー画像をバックバッファに描画
	cmd->SetPipelineState(dxBase->GetPipelineStateBlendModeAdd()); // 加算合成
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(0, materialCB_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(1, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(2, cmd, bloomBlurGH_);

	cmd->DrawIndexedInstanced(6, 1, 0, 0, 0);
#pragma endregion
}
