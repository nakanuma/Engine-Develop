#include "PostEffectManager.h"

// Engine
#include <ParticleEffect/ParticleEffectManager.h>
#include <RTVManager.h>
#include <Sprite.h>

void PostEffectManager::Initialize() {
	// 既に初期化済みならスキップ
	if (initialized_) return;
	initialized_ = true;

	DirectXBase* dxBase = DirectXBase::GetInstance();

	// レンダーテクスチャ作成
	renderTextureHandle_ = RTVManager::CreateRenderTargetTexture(Window::GetWidth(), Window::GetHeight());

	// 頂点バッファ
	vertexBuffer_ = CreateBufferResource(dxBase->GetDevice(), sizeof(Sprite::VertexData) * kVertexCount);
	vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeof(Sprite::VertexData) * kVertexCount;
	vbView_.StrideInBytes = sizeof(Sprite::VertexData);

	Sprite::VertexData* vbData = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vbData));
	vbData[0] = {
		{kNDCMin, kNDCMin, kNDCZ, kNDCW},
		{kUVMin, kUVMax},
		{kNormalX, kNormalY, kNormalZ}
	};
	vbData[1] = {
		{kNDCMin, kNDCMax, kNDCZ, kNDCW},
		{kUVMin, kUVMin},
		{kNormalX, kNormalY, kNormalZ}
	};
	vbData[2] = {
		{kNDCMax, kNDCMin, kNDCZ, kNDCW},
		{kUVMax, kUVMax},
		{kNormalX, kNormalY, kNormalZ}
	};
	vbData[3] = {
		{kNDCMax, kNDCMax, kNDCZ, kNDCW},
		{kUVMax, kUVMin},
		{kNormalX, kNormalY, kNormalZ}
	};

	// インデックスバッファ
	indexBuffer_ = CreateBufferResource(dxBase->GetDevice(), sizeof(uint32_t) * kIndexCount);
	ibView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	ibView_.SizeInBytes = sizeof(uint32_t) * kIndexCount;
	ibView_.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* ibData = nullptr;
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&ibData));
	ibData[0] = kIndex0;
	ibData[1] = kIndex1;
	ibData[2] = kIndex2;
	ibData[3] = kIndex1;
	ibData[4] = kIndex3;
	ibData[5] = kIndex2;

	// Transformation CB
	transformCB_ = CreateBufferResource(dxBase->GetDevice(), sizeof(Object3D::TransformationMatrix));
	transformCB_->Map(0, nullptr, reinterpret_cast<void**>(&transformMap_));
	transformMap_->WVP = Matrix::Identity();

	// Material CB
	materialCB_ = CreateBufferResource(dxBase->GetDevice(), sizeof(Object3D::Material));
	materialCB_->Map(0, nullptr, reinterpret_cast<void**>(&materialMap_));
	materialMap_->color = kDefaultMaterialColor;
	materialMap_->enableLighting = false;
	materialMap_->uvTransform = Matrix::Identity();

	/*アウトライン*/

	// レンダーテクスチャ
	outlineRT_ = RTVManager::CreateRenderTargetTexture(Window::GetWidth(), Window::GetHeight(), kOutlineClearColor);
	outlineGH_ = RTVManager::CreateRenderTargetTexture(Window::GetWidth(), Window::GetHeight(), kOutlineClearColor);

	// Outline Material
	outlineMaterial_.data_->color = kOutlineMaterialColor;
	outlineMaterial_.data_->enableLighting = false;
	outlineMaterial_.data_->uvTransform = Matrix::Identity();

	/*Bloom*/
	bloomExtractGH_ = RTVManager::CreateRenderTargetTexture(Window::GetWidth(), Window::GetHeight(), kBloomClearColor);
	bloomBlurGH_ = RTVManager::CreateRenderTargetTexture(Window::GetWidth(), Window::GetHeight(), kBloomClearColor);

	/*WaveDistortion*/
	waveCB_.data_->gTime = kWaveTimeInitial;
	waveCB_.data_->amplitude = kWaveAmplitudeInitial;
	waveCB_.data_->frequency = kWaveFrequencyInitial;
	waveCB_.data_->speed = kWaveSpeedInitial;

	/*GlitchEffect*/
	glitchCB_.data_->gTime = kGlitchTimeInitial;
	glitchCB_.data_->intensity = kGlitchIntensityInitial;
	glitchCB_.data_->speed = kGlitchSpeedInitial;
}

void PostEffectManager::TransfarConstantBuffer() {
	/*WaveDistrotion*/
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(kRootParameterIndexWave, waveCB_.resource_->GetGPUVirtualAddress());
	/*GlitchEffect*/
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(kRootParameterIndexGlitch, glitchCB_.resource_->GetGPUVirtualAddress());
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

	// レンダーターゲットをバックバッファに設定
	RTVManager::SetRTtoBB();

	if (effectType_ == PostEffectType::None) {
		return;
	}

	// 選択中のタイプによって適用するPSOを選択
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

	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexTransform, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(kRootParameterIndexTexture, cmd, renderTextureHandle_);

	cmd->DrawIndexedInstanced(kDrawIndexedCount, kInstancedCount, 0, 0, 0);
}

void PostEffectManager::BeginRenderToOutlineTexture() {
	// レンダーターゲットをアウトライン用テクスチャに設定
	RTVManager::SetRenderTarget(outlineRT_);
	// レンダーターゲットをクリア
	RTVManager::ClearRTV(outlineRT_, kOutlineClearColor);
}

void PostEffectManager::ApplyOutline() {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	auto cmd = dxBase->GetCommandList();

#pragma region 深度値を元にアウトライン生成
	// レンダーターゲットをアウトライン用テクスチャに設定
	RTVManager::SetRenderTarget(outlineGH_);
	// レンダーターゲットをクリア
	RTVManager::ClearRTV(outlineGH_, kOutlineClearColor);

	cmd->SetPipelineState(dxBase->GetPipelineStateSobelFilter()); // SobelFilterのPSOを設定
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexMaterial, outlineMaterial_.resource_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexTransform, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(kRootParameterIndexTexture, cmd, RTVManager::GetDepthSRVHandle(outlineRT_));

	cmd->DrawIndexedInstanced(kDrawIndexedCount, kInstancedCount, 0, 0, 0);
#pragma endregion
}

void PostEffectManager::DrawOutline() {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	auto cmd = dxBase->GetCommandList();

#pragma region renderTextureGHをバックバッファにそのまま描画
	// レンダーターゲットをバックバッファに設定
	RTVManager::SetRTtoBB();

	cmd->SetPipelineState(dxBase->GetPipelineState());
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexMaterial, materialCB_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexTransform, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(kRootParameterIndexTexture, cmd, renderTextureHandle_);

	cmd->DrawIndexedInstanced(kDrawIndexedCount, kInstancedCount, 0, 0, 0);
#pragma endregion

#pragma region outlineをバックバッファにそのまま描画
	cmd->SetPipelineState(dxBase->GetPipelineState()); // 通常PSOに戻す
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexMaterial, materialCB_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexTransform, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(kRootParameterIndexTexture, cmd, outlineGH_);

	cmd->DrawIndexedInstanced(kDrawIndexedCount, kInstancedCount, 0, 0, 0);
#pragma endregion
}

void PostEffectManager::ApplyBloom() {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	auto cmd = dxBase->GetCommandList();

#pragma region 明るい部分の抽出
	// レンダーターゲットをブルーム抽出用のテクスチャに設定
	RTVManager::SetRenderTarget(bloomExtractGH_);
	// レンダーターゲットをクリア
	RTVManager::ClearRTV(bloomExtractGH_, kBloomClearColor);

	cmd->SetPipelineState(dxBase->GetPipelineStateBloomExtract()); // ブルーム抽出用PSOを設定
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexMaterial, outlineMaterial_.resource_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexTransform, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(kRootParameterIndexTexture, cmd, renderTextureHandle_);

	cmd->DrawIndexedInstanced(kDrawIndexedCount, kInstancedCount, 0, 0, 0);
#pragma endregion

	ParticleEffectManager::GetInstance()->Draw();

#pragma region ガウスブラー
	// レンダーターゲットをブルームブラーテクスチャに設定
	RTVManager::SetRenderTarget(bloomBlurGH_);
	// レンダーターゲットをクリア
	RTVManager::ClearRTV(bloomBlurGH_, kBloomClearColor);

	cmd->SetPipelineState(dxBase->GetPipelineStateGaussianFilter());
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexMaterial, outlineMaterial_.resource_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexTransform, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(kRootParameterIndexTexture, cmd, bloomExtractGH_); // 明るさ抽出結果にブラーをかける

	cmd->DrawIndexedInstanced(kDrawIndexedCount, kInstancedCount, 0, 0, 0);
#pragma endregion
}

void PostEffectManager::DrawBloom() {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	auto cmd = dxBase->GetCommandList();

#pragma region renderTextureGHをバックバッファに描画
	// レンダーターゲットをバックバッファに設定
	RTVManager::SetRTtoBB();

	cmd->SetPipelineState(dxBase->GetPipelineState()); // 通常PSOに戻す
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexMaterial, materialCB_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexTransform, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(kRootParameterIndexTexture, cmd, renderTextureHandle_);

	cmd->DrawIndexedInstanced(kDrawIndexedCount, kInstancedCount, 0, 0, 0);
#pragma endregion

#pragma region ブラー画像をバックバッファに描画
	cmd->SetPipelineState(dxBase->GetPipelineStateBlendModeAdd()); // 加算合成を行う
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexMaterial, materialCB_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexTransform, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(kRootParameterIndexTexture, cmd, bloomBlurGH_);

	cmd->DrawIndexedInstanced(kDrawIndexedCount, kInstancedCount, 0, 0, 0);
#pragma endregion
}
