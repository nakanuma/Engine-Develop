#include "SpriteCommon.h"
#include "Logger.h"
#include <cassert>
#include "DirectXUtil.h"

void SpriteCommon::Initialize(DirectXBase* dxBase)
{
	// 引数で受け取ってメンバ変数に記録する
	dxBase_ = dxBase;

	// ルートシグネチャの作成
	CreateRootSignature();
	// InputLayoutの設定
	SetInputLayout();
	// DXC初期化
	InitializeDXC();
	// Shaderのコンパイル
	ShaderCompile();
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

void SpriteCommon::PreDraw()
{
	// ルートシグネチャをセット
	dxBase_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	// グラフィックスパイプラインステートをセット
	dxBase_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	// プリミティブトポロジーをセット
	dxBase_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void SpriteCommon::CreateRootSignature()
{
	HRESULT result = S_FALSE;

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// DescriptorRange作成
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
	descriptorRange[0].NumDescriptors = 1; // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	// RootParameter作成。複数設定できるので配列
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号0を使う

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; // Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで利用する数

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1; // レジスタ番号1を使う

	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[4].Descriptor.ShaderRegister = 2;

	descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	// Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0; // レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
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
	result = dxBase_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));
}

void SpriteCommon::CreateGraphicsPipeline()
{
	HRESULT result = S_FALSE;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get(); // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_; // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize() }; // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() }; // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc_; // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_; // RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
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

	//　無し
	graphicsPipelineStateDesc.BlendState = blendDescNone_;
	graphicsPipelineStateBlendModeNone_ = nullptr;
	result = dxBase_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineStateBlendModeNone_));
	//　加算
	graphicsPipelineStateDesc.BlendState = blendDescAdd_;
	graphicsPipelineStateBlendModeAdd_ = nullptr;
	result = dxBase_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineStateBlendModeAdd_));
	//　減算
	graphicsPipelineStateDesc.BlendState = blendDescSubtract_;
	graphicsPipelineStateBlendModeSubtract_ = nullptr;
	result = dxBase_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineStateBlendModeSubtract_));
	//　乗算
	graphicsPipelineStateDesc.BlendState = blendDescMultiply_;
	graphicsPipelineStateBlendModeMultiply_ = nullptr;
	result = dxBase_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineStateBlendModeMultiply_));
	//　スクリーン
	graphicsPipelineStateDesc.BlendState = blendDescScreen_;
	graphicsPipelineStateBlendModeScreen_ = nullptr;
	result = dxBase_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineStateBlendModeScreen_));
}

void SpriteCommon::SetInputLayout()
{
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[1].SemanticName = "TEXCOORD";
	inputElementDescs_[1].SemanticIndex = 0;
	inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[2].SemanticName = "NORMAL";
	inputElementDescs_[2].SemanticIndex = 0;
	inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);
}

void SpriteCommon::InitializeDXC()
{
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

void SpriteCommon::ShaderCompile()
{
	// Shaderをコンパイルする
	vertexShaderBlob_ = CompileShader(L"resources/Shaders/Sprite.VS.hlsl", L"vs_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = CompileShader(L"resources/Shaders/Sprite.PS.hlsl", L"ps_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(pixelShaderBlob_ != nullptr);
}

D3D12_RASTERIZER_DESC SpriteCommon::SetRasterizerState()
{
	// 裏面（時計回り）を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

	return rasterizerDesc_;
}

void SpriteCommon::CreateDepthBuffer()
{
	// DepthStencilTextureをウィンドウのサイズで作成
	depthStencilResource_ = CreateDepthStencilTextureResource(dxBase_->GetDevice(), Window::GetWidth(), Window::GetHeight(), false);

	// DSVの生成
	dsvDescriptorHeap_.Create(dxBase_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	// DSVの設定
	dsvDesc_.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Format。基本的にはResourceに合わせる
	dsvDesc_.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2dTexture;
	// DSVHeapの先頭にDSVをつくる
	dxBase_->GetDevice()->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc_, dsvDescriptorHeap_.GetCPUHandle(0));

	// DepthStencilStateの設定
	// Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = true;
	// 書き込みします
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

D3D12_BLEND_DESC SpriteCommon::SetBlendState()
{
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_.RenderTarget[0].BlendEnable = TRUE;
	blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	return blendDesc_;
}

D3D12_BLEND_DESC SpriteCommon::SetBlendStateNone()
{
	blendDescNone_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return blendDescNone_;
}

D3D12_BLEND_DESC SpriteCommon::SetBlendStateAdd()
{
	blendDescAdd_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescAdd_.RenderTarget[0].BlendEnable = TRUE;
	blendDescAdd_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDescAdd_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescAdd_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDescAdd_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescAdd_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescAdd_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	return blendDescAdd_;
}

D3D12_BLEND_DESC SpriteCommon::SetBlendStateSubtract()
{
	blendDescSubtract_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescSubtract_.RenderTarget[0].BlendEnable = TRUE;
	blendDescSubtract_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDescSubtract_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDescSubtract_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDescSubtract_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescSubtract_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescSubtract_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	return blendDescSubtract_;
}

D3D12_BLEND_DESC SpriteCommon::SetBlendStateMultiply()
{
	blendDescMultiply_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescMultiply_.RenderTarget[0].BlendEnable = TRUE;
	blendDescMultiply_.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDescMultiply_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescMultiply_.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	blendDescMultiply_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescMultiply_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescMultiply_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	return blendDescMultiply_;
}

D3D12_BLEND_DESC SpriteCommon::SetBlendStateScreen()
{
	blendDescScreen_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescScreen_.RenderTarget[0].BlendEnable = TRUE;
	blendDescScreen_.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDescScreen_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescScreen_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDescScreen_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescScreen_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescScreen_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	return blendDescScreen_;
}
