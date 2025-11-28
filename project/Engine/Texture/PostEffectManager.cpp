#include "PostEffectManager.h"

// Engine
#include <ParticleEffect/ParticleEffectManager.h>
#include <RTVManager.h>
#include <Sprite.h>

void PostEffectManager::Initialize() {
	// 初期化済みならスキップ
	if (initialized_) return;
	initialized_ = true;

	DirectXBase* dxBase = DirectXBase::GetInstance();

	// レンダーターゲット作成
	mainSceneRT_ = RTVManager::CreateRenderTargetTexture(Window::GetWidth(), Window::GetHeight());
	bloomResultRT_ = RTVManager::CreateRenderTargetTexture(Window::GetWidth(), Window::GetHeight(), kTransparentClearColor);
	bloomExtractRT_ = RTVManager::CreateRenderTargetTexture(Window::GetWidth(), Window::GetHeight(), kTransparentClearColor);
	bloomBlurRT_ = RTVManager::CreateRenderTargetTexture(Window::GetWidth(), Window::GetHeight(), kTransparentClearColor);

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

	// Black Material
	blackMaterial_.data_->color = kBlackMaterialColor;
	blackMaterial_.data_->enableLighting = false;
	blackMaterial_.data_->uvTransform = Matrix::Identity();

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

void PostEffectManager::BeginMainScene() {
	// エフェクトなしの場合はバックバッファに直接描画
	if (effectType_ == PostEffectType::None) return;

	// オフスクリーンレンダーターゲットに切り替え
	RTVManager::SetRenderTarget(mainSceneRT_);
	RTVManager::ClearRTV(mainSceneRT_);
	isRenderingToOffscreen_ = true;
}

void PostEffectManager::EndMainScene() {
	if (effectType_ == PostEffectType::None) return;

	DirectXBase* dxBase = DirectXBase::GetInstance();
	auto cmd = dxBase->GetCommandList();

	// バックバッファに切り替え
	RTVManager::SetRTtoBB();
	isRenderingToOffscreen_ = false;

	// エフェクトPSOを選択
	ID3D12PipelineState* pso = nullptr;
	switch (effectType_) {
	case PostEffectType::RadialBlur:
		pso = dxBase->GetPipelineStateRadialBlur();
		break;
	case PostEffectType::GrayScale:
		pso = dxBase->GetPipelineStateGrayscale();
		break;
	case PostEffectType::Vignette:
		pso = dxBase->GetPipelineStateVignette();
		break;
	case PostEffectType::BoxFilter:
		pso = dxBase->GetPipelineStateBoxFilter();
		break;
	case PostEffectType::GaussianFilter:
		pso = dxBase->GetPipelineStateGaussianFilter();
		break;
	case PostEffectType::InvertColor:
		pso = dxBase->GetPipelineStateInvertColor();
		break;
	case PostEffectType::Sepia:
		pso = dxBase->GetPipelineStateSepia();
		break;
	case PostEffectType::Posterize:
		pso = dxBase->GetPipelineStatePosterize();
		break;
	case PostEffectType::Emboss:
		pso = dxBase->GetPipelineStateEmboss();
		break;
	case PostEffectType::Sharpen:
		pso = dxBase->GetPipelineStateSharpen();
		break;
	case PostEffectType::ColorAberration:
		pso = dxBase->GetPipelineStateColorAberration();
		break;
	case PostEffectType::BarrelDistortion:
		pso = dxBase->GetPipelineStateBarrelDistortion();
		break;
	case PostEffectType::WaveDistortion:
		pso = dxBase->GetPipelineStateWaveDistortion();
		break;
	case PostEffectType::Pixelation:
		pso = dxBase->GetPipelineStatePixelation();
		break;
	case PostEffectType::GlitchEffect:
		pso = dxBase->GetPipelineStateGlitchEffect();
		break;
	default:
		pso = dxBase->GetPipelineState();
		break;
	}

	// エフェクト適用してバックバッファに適用
	DrawWithPSO(pso, mainSceneRT_);

	// 通常PSOに戻す
	cmd->SetPipelineState(dxBase->GetPipelineState());
}

void PostEffectManager::BeginBloom() {
	// Bloom結果用のレンダーターゲットに切り替え
	RTVManager::SetRenderTarget(bloomResultRT_);
	RTVManager::ClearRTV(bloomResultRT_, kTransparentClearColor);
	isRenderingToOffscreen_ = true;
}

void PostEffectManager::EndBloom() { 
	DirectXBase* dxBase = DirectXBase::GetInstance();
	auto cmd = dxBase->GetCommandList();

	// 明度抽出
	ApplyBloomExtract(bloomResultRT_, bloomExtractRT_);
	// ガウシアンブラー適用
	ApplyBloomBlur(bloomResultRT_, bloomBlurRT_);
	// バックバッファに戻す
	RTVManager::SetRTtoBB();
	isRenderingToOffscreen_ = false;
	// 元のシーンを描画
	DrawWithPSO(dxBase->GetPipelineState(), bloomResultRT_);
	// ブラー結果を加算合成
	DrawWithPSO(dxBase->GetPipelineStateBlendModeAdd(), bloomBlurRT_);
	// 通常PSOに戻す
	cmd->SetPipelineState(dxBase->GetPipelineState());
}

void PostEffectManager::RestoreBackBuffer(bool resetPSO) {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	auto cmd = dxBase->GetCommandList();

	UINT backBufferIndex = dxBase->GetSwapChain()->GetCurrentBackBufferIndex();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = dxBase->GetRTVHandle(backBufferIndex);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = RTVManager::GetDSVHandle(mainSceneRT_);
	ID3D12Resource* depthBufferResource = RTVManager::GetDepthResource(mainSceneRT_);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = depthBufferResource;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;

	cmd->ResourceBarrier(1, &barrier);

	cmd->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	if (resetPSO) {
		cmd->SetPipelineState(dxBase->GetPipelineState());
	}

	isRenderingToOffscreen_ = false;
}

void PostEffectManager::RestoreDepthBufferState()
{
	ID3D12Resource* depthBufferResource = RTVManager::GetDepthResource(mainSceneRT_);
	auto cmd = DirectXBase::GetInstance()->GetCommandList();

	D3D12_RESOURCE_BARRIER returnBarrier = {};
	returnBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	returnBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	returnBarrier.Transition.pResource = depthBufferResource;
	returnBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	returnBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	returnBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	cmd->ResourceBarrier(1, &returnBarrier);
}

void PostEffectManager::DrawFullScreenQuad(uint32_t textureHandle) { 
	auto cmd = DirectXBase::GetInstance()->GetCommandList(); 

	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexMaterial, materialCB_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexTransform, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(kRootParameterIndexTexture, cmd, textureHandle);
	cmd->DrawIndexedInstanced(kDrawIndexedCount, kInstancedCount, 0, 0, 0);
}

void PostEffectManager::DrawWithPSO(ID3D12PipelineState* pso, uint32_t textureHandle) { 
	auto cmd = DirectXBase::GetInstance()->GetCommandList(); 

	cmd->SetPipelineState(pso);
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexMaterial, materialCB_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexTransform, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(kRootParameterIndexTexture, cmd, textureHandle);
	cmd->DrawIndexedInstanced(kDrawIndexedCount, kInstancedCount, 0, 0, 0);
}

void PostEffectManager::ApplyBloomExtract(uint32_t sourceTexture, uint32_t targetRT) {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	auto cmd = dxBase->GetCommandList();

	RTVManager::SetRenderTarget(targetRT);
	RTVManager::ClearRTV(targetRT, kTransparentClearColor);

	cmd->SetPipelineState(dxBase->GetPipelineStateBloomExtract());
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexMaterial, materialCB_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexTransform, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(kRootParameterIndexTexture, cmd, sourceTexture);
	cmd->DrawIndexedInstanced(kDrawIndexedCount, kInstancedCount, 0, 0, 0);
}

void PostEffectManager::ApplyBloomBlur(uint32_t sourceTexture, uint32_t targetRT) {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	auto cmd = dxBase->GetCommandList();

	RTVManager::SetRenderTarget(targetRT);
	RTVManager::ClearRTV(targetRT, kTransparentClearColor);

	cmd->SetPipelineState(dxBase->GetPipelineStateGaussianFilter());
	cmd->IASetVertexBuffers(0, 1, &vbView_);
	cmd->IASetIndexBuffer(&ibView_);
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexMaterial, materialCB_->GetGPUVirtualAddress());
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexTransform, transformCB_->GetGPUVirtualAddress());
	TextureManager::SetDescriptorTable(kRootParameterIndexTexture, cmd, sourceTexture);
	cmd->DrawIndexedInstanced(kDrawIndexedCount, kInstancedCount, 0, 0, 0);
}