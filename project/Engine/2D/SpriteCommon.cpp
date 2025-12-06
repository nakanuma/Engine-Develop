#include "SpriteCommon.h"
#include "DirectXUtil.h"
#include "Logger.h"
#include "SRVManager.h"
#include <cassert>

void Cygnus::SpriteCommon::Initialize(DirectXBase* dxBase) {
	// 引数で受け取ってメンバ変数に記録する
	dxBase_ = dxBase;

	// ルートシグネチャの作成
	CreateRootSignature();
	// InputLayoutの設定
	SetInputLayout();
	// DXC初期化
	InitializeDXC();
	// RasterizerStateの設定
	SetRasterizerState();
	// 深度バッファ生成
	CreateDepthBuffer();
	// 各種ブレンドステートの設定
	SetBlendState();
	SetBlendStateNone();
	SetBlendStateAdd();
	SetBlendStateSubtract();
	SetBlendStateMultiply();
	SetBlendStateScreen();
	// グラフィックスパイプラインの生成
	CreateGraphicsPipeline();
}

void Cygnus::SpriteCommon::PreDraw() {
	// ルートシグネチャをセット
	dxBase_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	// グラフィックスパイプラインステートをセット
	dxBase_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	// プリミティブトポロジーをセット
	dxBase_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Cygnus::SpriteCommon::PostDraw() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	ID3D12DescriptorHeap* descriptorHeaps[] = { SRVManager::GetInstance()->descriptorHeap_.heap_.Get() };

	// ルートシグネチャをセット
	dxBase->GetCommandList()->SetGraphicsRootSignature(dxBase->GetRootSignature());
	// 通常PSOをセット
	dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineState());
	// プリミティブトポロジーをセット
	dxBase->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ディスクリプタヒープをセット
	dxBase->GetCommandList()->SetDescriptorHeaps(kDescriptorHeapCount, descriptorHeaps);
}

void Cygnus::SpriteCommon::CreateRootSignature() {
	HRESULT result = S_FALSE;

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// DescriptorRange作成
	D3D12_DESCRIPTOR_RANGE descriptorRange[kDescriptorRangeCount] = {};
	descriptorRange[kRootParameterIndexMaterial].BaseShaderRegister = kDescriptorRangeBaseShaderRegister;
	descriptorRange[kRootParameterIndexMaterial].NumDescriptors = kDescriptorRangeNumDescriptors;
	descriptorRange[kRootParameterIndexMaterial].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[kRootParameterIndexMaterial].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// RootParameter作成。複数設定できるので配列
	D3D12_ROOT_PARAMETER rootParameters[kRootParameterCount] = {};
	rootParameters[kRootParameterIndexMaterial].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[kRootParameterIndexMaterial].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[kRootParameterIndexMaterial].Descriptor.ShaderRegister = kMaterialShaderRegister;

	rootParameters[kRootParameterIndexTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[kRootParameterIndexTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[kRootParameterIndexTransform].Descriptor.ShaderRegister = kTransformShaderRegister;

	rootParameters[kRootParameterIndexTexture].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;      // DescriptorTableを使う
	rootParameters[kRootParameterIndexTexture].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;                // PixelShaderで使う
	rootParameters[kRootParameterIndexTexture].DescriptorTable.pDescriptorRanges = descriptorRange;             // Tableの中身の配列を指定
	rootParameters[kRootParameterIndexTexture].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで利用する数

	rootParameters[kRootParameterIndexLight].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[kRootParameterIndexLight].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[kRootParameterIndexLight].Descriptor.ShaderRegister = kLightShaderRegister;

	rootParameters[kRootParameterIndexCamera].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[kRootParameterIndexCamera].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[kRootParameterIndexCamera].Descriptor.ShaderRegister = kCameraShaderRegister;

	descriptionRootSignature.pParameters = rootParameters;             // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	// Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[kStaticSamplerCount] = {};
	staticSamplers[kRootParameterIndexMaterial].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;   // バイリニアフィルタ
	staticSamplers[kRootParameterIndexMaterial].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	staticSamplers[kRootParameterIndexMaterial].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[kRootParameterIndexMaterial].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[kRootParameterIndexMaterial].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;     // 比較しない
	staticSamplers[kRootParameterIndexMaterial].MaxLOD = D3D12_FLOAT32_MAX;                       // ありったけのMipmapを使う
	staticSamplers[kRootParameterIndexMaterial].ShaderRegister = kSamplerShaderRegister;
	staticSamplers[kRootParameterIndexMaterial].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	signatureBlob_ = nullptr;
	errorBlob_ = nullptr;
	result = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(result)) {
		Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	rootSignature_ = nullptr;
	result = dxBase_->GetDevice()->CreateRootSignature(kRootSignatureVersion, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));
}

void Cygnus::SpriteCommon::CreateGraphicsPipeline() {
	HRESULT result = S_FALSE;

	ShaderManager* shaderManager = ShaderManager::GetInstance();
	auto vs = shaderManager->GetShader("Sprite_VS");
	auto ps = shaderManager->GetShader("Sprite_PS");

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();              // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;                     // InputLayout
	graphicsPipelineStateDesc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() }; // VertexShader
	graphicsPipelineStateDesc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() }; // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc_;                            // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;                  // RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = kRenderTargetCount;
	graphicsPipelineStateDesc.RTVFormats[kRenderTargetIndex] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = kSampleDescCount;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc_;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	graphicsPipelineState_ = nullptr;
	result = dxBase_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(result));

	///
	/// BlendMode変更用のPSOを生成
	///

	// 　無し
	graphicsPipelineStateDesc.BlendState = blendDescNone_;
	graphicsPipelineStateBlendModeNone_ = nullptr;
	result = dxBase_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineStateBlendModeNone_));
	// 　加算
	graphicsPipelineStateDesc.BlendState = blendDescAdd_;
	graphicsPipelineStateBlendModeAdd_ = nullptr;
	result = dxBase_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineStateBlendModeAdd_));
	// 　減算
	graphicsPipelineStateDesc.BlendState = blendDescSubtract_;
	graphicsPipelineStateBlendModeSubtract_ = nullptr;
	result = dxBase_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineStateBlendModeSubtract_));
	// 　乗算
	graphicsPipelineStateDesc.BlendState = blendDescMultiply_;
	graphicsPipelineStateBlendModeMultiply_ = nullptr;
	result = dxBase_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineStateBlendModeMultiply_));
	// 　スクリーン
	graphicsPipelineStateDesc.BlendState = blendDescScreen_;
	graphicsPipelineStateBlendModeScreen_ = nullptr;
	result = dxBase_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineStateBlendModeScreen_));
}

void Cygnus::SpriteCommon::SetInputLayout() {
	inputElementDescs_[kInputElementIndexPositon].SemanticName = "POSITION";
	inputElementDescs_[kInputElementIndexPositon].SemanticIndex = kSemanticIndex;
	inputElementDescs_[kInputElementIndexPositon].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[kInputElementIndexPositon].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[kInputElementIndexTexcoord].SemanticName = "TEXCOORD";
	inputElementDescs_[kInputElementIndexTexcoord].SemanticIndex = kSemanticIndex;
	inputElementDescs_[kInputElementIndexTexcoord].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs_[kInputElementIndexTexcoord].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[kInputElementIndexNormal].SemanticName = "NORMAL";
	inputElementDescs_[kInputElementIndexNormal].SemanticIndex = kSemanticIndex;
	inputElementDescs_[kInputElementIndexNormal].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[kInputElementIndexNormal].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);
}

void Cygnus::SpriteCommon::InitializeDXC() {
	HRESULT result = S_FALSE;

	// dxcCompilerを初期化
	dxcUtils_ = nullptr;
	dxcCompiler_ = nullptr;
	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(result));
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(result));

	// 現時点でincludeはしないが、includeに対応するための設定を行っておく
	includeHandler_ = nullptr;
	result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(result));
}

D3D12_RASTERIZER_DESC Cygnus::SpriteCommon::SetRasterizerState() {
	// 裏面（時計回り）を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

	return rasterizerDesc_;
}

void Cygnus::SpriteCommon::CreateDepthBuffer() {
	// DepthStencilTextureをウィンドウのサイズで作成
	depthStencilResource_ = CreateDepthStencilTextureResource(dxBase_->GetDevice(), Window::GetWidth(), Window::GetHeight(), false);

	// DSVの生成
	dsvDescriptorHeap_.Create(dxBase_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, kDSVDescriptorCount, false);

	// DSVの設定
	dsvDesc_.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;        // Format。基本的にはResourceに合わせる
	dsvDesc_.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2dTexture;
	// DSVHeapの先頭にDSVをつくる
	dxBase_->GetDevice()->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc_, dsvDescriptorHeap_.GetCPUHandle(kDSVHeapIndex));

	// DepthStencilStateの設定
	// Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = false;
	// 書き込みします
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

D3D12_BLEND_DESC Cygnus::SpriteCommon::SetBlendState() {
	blendDesc_.RenderTarget[kRenderTargetIndex].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_.RenderTarget[kRenderTargetIndex].BlendEnable = TRUE;
	blendDesc_.RenderTarget[kRenderTargetIndex].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_.RenderTarget[kRenderTargetIndex].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[kRenderTargetIndex].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc_.RenderTarget[kRenderTargetIndex].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[kRenderTargetIndex].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[kRenderTargetIndex].DestBlendAlpha = D3D12_BLEND_ZERO;

	return blendDesc_;
}

D3D12_BLEND_DESC Cygnus::SpriteCommon::SetBlendStateNone() {
	blendDescNone_.RenderTarget[kRenderTargetIndex].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return blendDescNone_;
}

D3D12_BLEND_DESC Cygnus::SpriteCommon::SetBlendStateAdd() {
	blendDescAdd_.RenderTarget[kRenderTargetIndex].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescAdd_.RenderTarget[kRenderTargetIndex].BlendEnable = TRUE;
	blendDescAdd_.RenderTarget[kRenderTargetIndex].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDescAdd_.RenderTarget[kRenderTargetIndex].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescAdd_.RenderTarget[kRenderTargetIndex].DestBlend = D3D12_BLEND_ONE;
	blendDescAdd_.RenderTarget[kRenderTargetIndex].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescAdd_.RenderTarget[kRenderTargetIndex].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescAdd_.RenderTarget[kRenderTargetIndex].DestBlendAlpha = D3D12_BLEND_ZERO;

	return blendDescAdd_;
}

D3D12_BLEND_DESC Cygnus::SpriteCommon::SetBlendStateSubtract() {
	blendDescSubtract_.RenderTarget[kRenderTargetIndex].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescSubtract_.RenderTarget[kRenderTargetIndex].BlendEnable = TRUE;
	blendDescSubtract_.RenderTarget[kRenderTargetIndex].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDescSubtract_.RenderTarget[kRenderTargetIndex].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDescSubtract_.RenderTarget[kRenderTargetIndex].DestBlend = D3D12_BLEND_ONE;
	blendDescSubtract_.RenderTarget[kRenderTargetIndex].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescSubtract_.RenderTarget[kRenderTargetIndex].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescSubtract_.RenderTarget[kRenderTargetIndex].DestBlendAlpha = D3D12_BLEND_ZERO;

	return blendDescSubtract_;
}

D3D12_BLEND_DESC Cygnus::SpriteCommon::SetBlendStateMultiply() {
	blendDescMultiply_.RenderTarget[kRenderTargetIndex].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescMultiply_.RenderTarget[kRenderTargetIndex].BlendEnable = TRUE;
	blendDescMultiply_.RenderTarget[kRenderTargetIndex].SrcBlend = D3D12_BLEND_ZERO;
	blendDescMultiply_.RenderTarget[kRenderTargetIndex].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescMultiply_.RenderTarget[kRenderTargetIndex].DestBlend = D3D12_BLEND_SRC_COLOR;
	blendDescMultiply_.RenderTarget[kRenderTargetIndex].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescMultiply_.RenderTarget[kRenderTargetIndex].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescMultiply_.RenderTarget[kRenderTargetIndex].DestBlendAlpha = D3D12_BLEND_ZERO;

	return blendDescMultiply_;
}

D3D12_BLEND_DESC Cygnus::SpriteCommon::SetBlendStateScreen() {
	blendDescScreen_.RenderTarget[kRenderTargetIndex].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescScreen_.RenderTarget[kRenderTargetIndex].BlendEnable = TRUE;
	blendDescScreen_.RenderTarget[kRenderTargetIndex].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDescScreen_.RenderTarget[kRenderTargetIndex].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescScreen_.RenderTarget[kRenderTargetIndex].DestBlend = D3D12_BLEND_ONE;
	blendDescScreen_.RenderTarget[kRenderTargetIndex].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescScreen_.RenderTarget[kRenderTargetIndex].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescScreen_.RenderTarget[kRenderTargetIndex].DestBlendAlpha = D3D12_BLEND_ZERO;

	return blendDescScreen_;
}
