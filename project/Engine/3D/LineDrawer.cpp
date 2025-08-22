#include "LineDrawer.h"

// Engine
#include <Camera.h>
#include <ShaderManager.h>

LineDrawer* LineDrawer::GetInstance()
{
	static LineDrawer instance;
	return &instance;
}

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void LineDrawer::Initialize()
{
	dxBase_ = DirectXBase::GetInstance();

	CreateRootSignature();
	SetInputLayout();
	InitializeDXC();
	SetRasterizerState();
	CreateDepthBuffer();
	SetBlendState();
	CreateGraphicsPipeline();

	// 定数バッファ作成
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = (sizeof(TransformationMatrix) + 0xff) & ~0xff; // 256バイトアライン
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT hr = dxBase_->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constanceBuffer_)
	);
	assert(SUCCEEDED(hr));

	constanceBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&constMap_));
}

// ---------------------------------------------------------
// 線の登録
// ---------------------------------------------------------
void LineDrawer::RegisterLine(const Float3& start, const Float3& end, const Float4& color)
{
	vertices_.push_back({ start, color });
	vertices_.push_back({ end, color });
}

void LineDrawer::Render()
{
	if (vertices_.empty()) return;

	auto device = dxBase_->GetDevice();
	auto cmdList = dxBase_->GetCommandList();

	size_t vbSize = sizeof(Vertex) * vertices_.size();

	// 頂点バッファ作成
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = vbSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT hr = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexResource_)
	);
	assert(SUCCEEDED(hr));

	Vertex* vbData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vbData));
	std::memcpy(vbData, vertices_.data(), vbSize);
	vertexResource_->Unmap(0, nullptr);

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = static_cast<UINT>(vbSize);
	vertexBufferView_.StrideInBytes = sizeof(Vertex);

	// PSOセット
	cmdList->SetGraphicsRootSignature(rootSignature_.Get());
	cmdList->SetPipelineState(pipelineState_.Get());

	// トポロジ
	cmdList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	// WVP行列を取得
	Matrix viewMatrix = Camera::GetCurrent()->MakeViewMatrix();
	Matrix projectionMatrix = Camera::GetCurrent()->MakePerspectiveFovMatrix();
	Matrix vpMatrix = viewMatrix * projectionMatrix;
	constMap_->WVP = vpMatrix;
	cmdList->SetGraphicsRootConstantBufferView(0, constanceBuffer_->GetGPUVirtualAddress());

	// 描画
	cmdList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);

	vertices_.clear();
}

void LineDrawer::CreateRootSignature()
{
	D3D12_ROOT_PARAMETER rootParams[1] = {};
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParams[0].Descriptor.ShaderRegister = 0;
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	desc.NumParameters = _countof(rootParams);
	desc.pParameters = rootParams;

	Microsoft::WRL::ComPtr<ID3DBlob> sigBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errBlob;
	HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
	assert(SUCCEEDED(hr));

	hr = dxBase_->GetDevice()->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}

void LineDrawer::CreateGraphicsPipeline()
{
	HRESULT result = S_FALSE;

	ShaderManager* shaderManager = ShaderManager::GetInstance();
	auto vs = shaderManager->GetShader("LineDrawer_VS");
	auto ps = shaderManager->GetShader("LineDrawer_PS");

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rootSignature_.Get();
	psoDesc.InputLayout = inputLayoutDesc_;
	psoDesc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
	psoDesc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };
	psoDesc.BlendState = blendDesc_;
	psoDesc.RasterizerState = rasterizerDesc_;
	// 書き込むRTVの情報
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジのタイプ
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	// どのように画面に色を打ち込むかの設定
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencil
	psoDesc.DepthStencilState = depthStencilDesc_;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 生成
	pipelineState_ = nullptr;
	result = dxBase_->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));
}

void LineDrawer::SetInputLayout()
{
	// InputLayout
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[1].SemanticName = "COLOR";
	inputElementDescs_[1].SemanticIndex = 0;
	inputElementDescs_[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);
}

void LineDrawer::InitializeDXC()
{
	HRESULT result = S_FALSE;

	// dxcCompilerを初期化
	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(result));
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(result));
	// 現時点でincludeはしないが、includeに対応するための設定を行っておく
	result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(result));
}

D3D12_RASTERIZER_DESC LineDrawer::SetRasterizerState()
{
	// 裏面（時計回り）を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
	// 塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

	return rasterizerDesc_;
}

void LineDrawer::CreateDepthBuffer()
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

D3D12_BLEND_DESC LineDrawer::SetBlendState()
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