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
	lineVertices_.push_back({ start, color });
	lineVertices_.push_back({ end, color });
}

// ---------------------------------------------------------
// 扇形（塗りつぶし）の追加
// ---------------------------------------------------------
void LineDrawer::RegisterSector(const Float3& center, float innerRadius, float outerRadius, float startAngleRad, float endAngleRad, uint32_t segments, const Float4& innerColor, const Float4& outerColor, float yOffset)
{
	if (segments < 1) return;

	Float3 c = center;
	c.y += yOffset; // 少し浮かせる

	for (uint32_t i = 0; i < segments; ++i) {
		float t0 = static_cast<float>(i) / segments;
		float t1 = static_cast<float>(i + 1) / segments;

		float a0 = startAngleRad + (endAngleRad - startAngleRad) * t0;
		float a1 = startAngleRad + (endAngleRad - startAngleRad) * t1;

		// 内側弧
		Float3 i0 = { c.x + std::cosf(a0) * innerRadius, c.y, c.z + std::sinf(a0) * innerRadius };
		Float3 i1 = { c.x + std::cosf(a1) * innerRadius, c.y, c.z + std::sinf(a1) * innerRadius };

		// 外側弧
		Float3 o0 = { c.x + std::cosf(a0) * outerRadius, c.y, c.z + std::sinf(a0) * outerRadius };
		Float3 o1 = { c.x + std::cosf(a1) * outerRadius, c.y, c.z + std::sinf(a1) * outerRadius };

		// Quadを三角形2枚で作成
		triVertices_.push_back({ i0, innerColor });
		triVertices_.push_back({ o0, outerColor });
		triVertices_.push_back({ o1, outerColor });

		triVertices_.push_back({ i0, innerColor });
		triVertices_.push_back({ o1, outerColor });
		triVertices_.push_back({ i1, innerColor });
	}
}

// ---------------------------------------------------------
// トレーサー線の登録
// ---------------------------------------------------------
void LineDrawer::RegisterTracer(const Float3& start, const Float3& end, float thickness, const Float4& headColor, const Float4& tailColor) {
	// 方向ベクトルと長さ
	Float3 dir = end - start;
	float len = Float3::Length(dir);
	if (len < 0.001f) return; // 長さ0なら描画しない
	dir = Float3(dir.x / len, dir.y / len, dir.z / len);

	// カメラに対して垂直なオフセット方向を求める
	Float3 up = { 0.0f, 1.0f, 0.0f };
	Float3 side = Float3::Normalize(Float3::Cross(up, dir));
	Float3 offset = side * (thickness * 0.5f);

	// 四角形の4頂点
	Float3 v0 = start - offset; // tail left
	Float3 v1 = start + offset; // tail right
	Float3 v2 = end + offset; // head right;
	Float3 v3 = end - offset; // head left;

	// 三角形2毎にして分割して登録
	triVertices_.push_back({ v0, tailColor });
	triVertices_.push_back({ v1, tailColor });
	triVertices_.push_back({ v2, headColor });

	triVertices_.push_back({ v0, tailColor });
	triVertices_.push_back({ v2, headColor });
	triVertices_.push_back({ v3, headColor });
}

void LineDrawer::Draw()
{
	auto device = dxBase_->GetDevice();
	auto cmdList = dxBase_->GetCommandList();

	if (lineVertices_.empty() && triVertices_.empty()) return;

	// 定数バッファ（VP）
	Matrix viewMatrix = Camera::GetCurrent()->MakeViewMatrix();
	Matrix projectionMatrix = Camera::GetCurrent()->MakePerspectiveFovMatrix();
	constMap_->WVP = viewMatrix * projectionMatrix;

	// 共通セット
	cmdList->SetGraphicsRootSignature(rootSignature_.Get());
	cmdList->SetGraphicsRootConstantBufferView(0, constanceBuffer_->GetGPUVirtualAddress());

	///
	///	三角形
	/// 

	if (!triVertices_.empty()) {
		size_t vbSize = sizeof(Vertex) * triVertices_.size();

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

		device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&triVertexResource_)
		);

		Vertex* mapped = nullptr;
		triVertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mapped));
		std::memcpy(mapped, triVertices_.data(), vbSize);
		triVertexResource_->Unmap(0, nullptr);

		triVBV_.BufferLocation = triVertexResource_->GetGPUVirtualAddress();
		triVBV_.SizeInBytes = static_cast<UINT>(vbSize);
		triVBV_.StrideInBytes = sizeof(Vertex);

		// PSO + トポロジ
		cmdList->SetPipelineState(pipelineStateTri_.Get());
		cmdList->IASetVertexBuffers(0, 1, &triVBV_);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 描画
		cmdList->DrawInstanced(static_cast<UINT>(triVertices_.size()), 1, 0, 0);
	}

	///
	/// 線分
	/// 

	if (!lineVertices_.empty()) {
		size_t vbSize = sizeof(Vertex) * lineVertices_.size();

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

		device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&lineVertexResource_)
		);

		Vertex* vbData = nullptr;
		lineVertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vbData));
		std::memcpy(vbData, lineVertices_.data(), vbSize);
		lineVertexResource_->Unmap(0, nullptr);

		lineVBV_.BufferLocation = lineVertexResource_->GetGPUVirtualAddress();
		lineVBV_.SizeInBytes = static_cast<UINT>(vbSize);
		lineVBV_.StrideInBytes = sizeof(Vertex);

		// PSO + トポロジ
		cmdList->SetPipelineState(pipelineStateLine_.Get());
		cmdList->IASetVertexBuffers(0, 1, &lineVBV_);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		// 描画
		cmdList->DrawInstanced(static_cast<UINT>(lineVertices_.size()), 1, 0, 0);
	}

	///
	///	トレーサーストリップ（三角形帯）
	/// 

	if (!tracerStrip_.empty()) {
		size_t vbSize = sizeof(TrailVertex) * tracerStrip_.size();

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

		device->CreateCommittedResource(
			&heapProp, 
			D3D12_HEAP_FLAG_NONE, 
			&resDesc, 
			D3D12_RESOURCE_STATE_GENERIC_READ, 
			nullptr, 
			IID_PPV_ARGS(&tracerStripResource_)
		);

		TrailVertex* vbData = nullptr;
		tracerStripResource_->Map(0, nullptr, reinterpret_cast<void**>(&vbData));
		std::memcpy(vbData, tracerStrip_.data(), vbSize);
		tracerStripResource_->Unmap(0, nullptr);

		tracerStripVBV_.BufferLocation = tracerStripResource_->GetGPUVirtualAddress();
		tracerStripVBV_.SizeInBytes = static_cast<UINT>(vbSize);
		tracerStripVBV_.StrideInBytes = sizeof(TrailVertex);

		// PSO + トポロジ
		cmdList->SetPipelineState(pipelineStateTracer_.Get());
		cmdList->IASetVertexBuffers(0, 1, &tracerStripVBV_);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// 描画
		cmdList->DrawInstanced(static_cast<UINT>(tracerStrip_.size()), 1, 0, 0);
	}

	// クリア
	triVertices_.clear();
	lineVertices_.clear();
	tracerStrip_.clear();
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

	auto makeDesk = [&](D3D12_PRIMITIVE_TOPOLOGY_TYPE topo,
		const std::string& vsName,
		const std::string& psName,
		D3D12_INPUT_LAYOUT_DESC inputLayout) 
		{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC d{};
		d.pRootSignature = rootSignature_.Get();
		d.InputLayout = inputLayout;

		ShaderManager* shaderManager = ShaderManager::GetInstance();
		auto vs = shaderManager->GetShader(vsName);
		auto ps = shaderManager->GetShader(psName);
		d.VS = {vs->GetBufferPointer(), vs->GetBufferSize()};
		d.PS = {ps->GetBufferPointer(), ps->GetBufferSize()};
		d.BlendState = blendDesc_;
		d.RasterizerState = rasterizerDesc_;
		// 書き込むRTVの情報
		d.NumRenderTargets = 1;
		d.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// 利用するトポロジのタイプ
		d.PrimitiveTopologyType = topo;
		// どのように画面に色を打ち込むかの設定
		d.SampleDesc.Count = 1;
		d.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		// DepthStencil
		d.DepthStencilState = depthStencilDesc_;
		d.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		return d;
	};

	// 線分用
	D3D12_GRAPHICS_PIPELINE_STATE_DESC lineDesc = makeDesk(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, "LineDrawer_VS", "LineDrawer_PS", inputLayoutDesc_);
	dxBase_->GetDevice()->CreateGraphicsPipelineState(&lineDesc, IID_PPV_ARGS(&pipelineStateLine_));

	// 三角形用
	D3D12_GRAPHICS_PIPELINE_STATE_DESC triDesc = makeDesk(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, "LineDrawer_VS", "LineDrawer_PS", inputLayoutDesc_);
	dxBase_->GetDevice()->CreateGraphicsPipelineState(&triDesc, IID_PPV_ARGS(&pipelineStateTri_));

	// トレーサー用
	D3D12_GRAPHICS_PIPELINE_STATE_DESC tracerDesc = makeDesk(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, "LineDrawer_VS", "LineDrawer_PS", inputLayoutDesc_);
	dxBase_->GetDevice()->CreateGraphicsPipelineState(&tracerDesc, IID_PPV_ARGS(&pipelineStateTracer_));
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