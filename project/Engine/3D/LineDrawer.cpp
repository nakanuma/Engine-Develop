#include "LineDrawer.h"

// Engine
#include <Camera.h>
#include <ShaderManager.h>
#include <CommandManager.h>

template <typename T>
void Cygnus::LineDrawer::UpdateVertexBuffer(const std::vector<T>& vertices, Microsoft::WRL::ComPtr<ID3D12Resource>& resource, D3D12_VERTEX_BUFFER_VIEW& vbv){
	if(vertices.empty()) return;

	auto device = dxBase_->GetDevice();

	size_t vbSize = sizeof(T) * vertices.size();

	// 頂点バッファのディスクリプタ設定
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

	// リソース作成
	HRESULT result = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(result));

	// データ転送
	T* mapped = nullptr;
	resource->Map(0, nullptr, reinterpret_cast<void**>(&mapped));
	std::memcpy(mapped, vertices.data(), vbSize);
	resource->Unmap(0, nullptr);
	
	// VBV設定
	vbv.BufferLocation = resource->GetGPUVirtualAddress();
	vbv.SizeInBytes = static_cast<UINT>(vbSize);
	vbv.StrideInBytes = sizeof(T);
}

Cygnus::LineDrawer* Cygnus::LineDrawer::GetInstance() {
	static Cygnus::LineDrawer instance;
	return &instance;
}

void Cygnus::LineDrawer::Initialize() {
	dxBase_ = DirectXBase::GetInstance();

	CreateRootSignature();
	SetInputLayout();
	InitializeDXC();
	SetRasterizerState();
	CreateDepthBuffer();
	SetBlendState();
	// PSO生成
	CreatePipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, pipelineStateLine_);		// Line
	CreatePipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, pipelineStateTri_);		// Triangle
	CreatePipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, pipelineStateTracer_);	// Tracer

	// 定数バッファ作成
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = (sizeof(TransformationMatrix) + kConstBufferAlignment) & ~kConstBufferAlignment; // 256バイトアライン
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT hr = dxBase_->GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constanceBuffer_));
	assert(SUCCEEDED(hr));

	constanceBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&constMap_));
}

void Cygnus::LineDrawer::RegisterLine(const Float3& start, const Float3& end, const Float4& color) {
	lineVertices_.push_back({ start, color });
	lineVertices_.push_back({ end, color });
}

void Cygnus::LineDrawer::RegisterSector(
	const Float3& center, float innerRadius, float outerRadius, float startAngleRad, float endAngleRad, uint32_t segments, const Float4& innerColor, const Float4& outerColor, float yOffset) {
	if (segments < 1)
		return;

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

void Cygnus::LineDrawer::RegisterTracer(const Float3& start, const Float3& end, float thickness, const Float4& headColor, const Float4& tailColor) {
	// 方向ベクトルと長さ
	Float3 dir = end - start;
	float len = Float3::Length(dir);
	if (len < kTracerLengthEpsilon)
		return; // 長さ0なら描画しない
	dir = Float3(dir.x / len, dir.y / len, dir.z / len);

	// カメラに対して垂直なオフセット方向を求める
	Float3 up = { 0.0f, 1.0f, 0.0f };
	Float3 side = Float3::Normalize(Float3::Cross(up, dir));
	Float3 offset = side * (thickness * kTracerThicknessHalf);

	// 四角形の4頂点
	Float3 v0 = start - offset; // tail left
	Float3 v1 = start + offset; // tail right
	Float3 v2 = end + offset;   // head right;
	Float3 v3 = end - offset;   // head left;

	// 三角形2毎にして分割して登録
	triVertices_.push_back({ v0, tailColor });
	triVertices_.push_back({ v1, tailColor });
	triVertices_.push_back({ v2, headColor });

	triVertices_.push_back({ v0, tailColor });
	triVertices_.push_back({ v2, headColor });
	triVertices_.push_back({ v3, headColor });
}

void Cygnus::LineDrawer::Draw() {
	auto cmd = CommandManager::GetInstance()->GetCommandList();

	if (lineVertices_.empty() && triVertices_.empty())
		return;

	// 定数バッファ（VP）
	Matrix viewMatrix = Camera::GetCurrent()->MakeViewMatrix();
	Matrix projectionMatrix = Camera::GetCurrent()->MakePerspectiveFovMatrix();
	constMap_->WVP = viewMatrix * projectionMatrix;

	// 共通セット
	cmd->SetGraphicsRootSignature(rootSignature_.Get());
	cmd->SetGraphicsRootConstantBufferView(0, constanceBuffer_->GetGPUVirtualAddress());

	///
	///	三角形
	///

	if (!triVertices_.empty()) {
		// 頂点バッファの更新・生成
		UpdateVertexBuffer(triVertices_, triVertexResource_, triVBV_);

		// PSO + トポロジ設定
		cmd->SetPipelineState(pipelineStateTri_.Get());
		cmd->IASetVertexBuffers(0, 1, &triVBV_);
		cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 描画
		cmd->DrawInstanced(static_cast<UINT>(triVertices_.size()), 1, 0, 0);
	}

	///
	/// 線分
	///

	if (!lineVertices_.empty()) {
		// 頂点バッファの更新・生成
		UpdateVertexBuffer(lineVertices_, lineVertexResource_, lineVBV_);

		// PSO + トポロジ
		cmd->SetPipelineState(pipelineStateLine_.Get());
		cmd->IASetVertexBuffers(0, 1, &lineVBV_);
		cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		// 描画
		cmd->DrawInstanced(static_cast<UINT>(lineVertices_.size()), 1, 0, 0);
	}

	///
	///	トレーサーストリップ（三角形帯）
	///

	if (!tracerStrip_.empty()) {
		// 頂点バッファの更新・生成
		UpdateVertexBuffer(tracerStrip_, tracerStripResource_, tracerStripVBV_);

		// PSO + トポロジ
		cmd->SetPipelineState(pipelineStateTracer_.Get());
		cmd->IASetVertexBuffers(0, 1, &tracerStripVBV_);
		cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// 描画
		cmd->DrawInstanced(static_cast<UINT>(tracerStrip_.size()), 1, 0, 0);
	}

	// クリア
	triVertices_.clear();
	lineVertices_.clear();
	tracerStrip_.clear();
}

void Cygnus::LineDrawer::CreateRootSignature() {
	// ルートパラメーター配列を作成
	D3D12_ROOT_PARAMETER rootParams[1] = {};
	rootParams[kRootParameterIndexWVP].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // 定数バッファを渡す
	rootParams[kRootParameterIndexWVP].Descriptor.ShaderRegister = kWVPShaderRegister;                     // b0に対応
	rootParams[kRootParameterIndexWVP].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // 頂点シェーダーで使う

	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 入力アセンブラの使用を許可
	desc.NumParameters = _countof(rootParams);                                 // ルートパラメータ数
	desc.pParameters = rootParams;                                             // パラメーター配列へのポインタ

	Microsoft::WRL::ComPtr<ID3DBlob> sigBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errBlob;
	HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
	assert(SUCCEEDED(hr));

	hr = dxBase_->GetDevice()->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}

void Cygnus::LineDrawer::SetInputLayout() {
	// InputLayout
	inputElementDescs_[kInputElementIndexPositon].SemanticName = "POSITION";
	inputElementDescs_[kInputElementIndexPositon].SemanticIndex = kSemanticIndex;
	inputElementDescs_[kInputElementIndexPositon].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[kInputElementIndexPositon].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[kInputElementIndexColor].SemanticName = "COLOR";
	inputElementDescs_[kInputElementIndexColor].SemanticIndex = kSemanticIndex;
	inputElementDescs_[kInputElementIndexColor].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[kInputElementIndexColor].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);
}

void Cygnus::LineDrawer::InitializeDXC() {
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

D3D12_RASTERIZER_DESC Cygnus::LineDrawer::SetRasterizerState() {
	// 裏面（時計回り）を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
	// 塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

	return rasterizerDesc_;
}

void Cygnus::LineDrawer::CreateDepthBuffer() {
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
	depthStencilDesc_.DepthEnable = true;
	// 書き込みします
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

D3D12_BLEND_DESC Cygnus::LineDrawer::SetBlendState() {
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

void Cygnus::LineDrawer::CreatePipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE topo, Microsoft::WRL::ComPtr<ID3D12PipelineState>& pso)
{
	HRESULT result = S_FALSE;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = rootSignature_.Get();
	psoDesc.InputLayout = inputLayoutDesc_;

	ShaderManager* shaderManager = ShaderManager::GetInstance();
	auto vs = shaderManager->GetShader("LineDrawer_VS");
	auto ps = shaderManager->GetShader("LineDrawer_PS");
	psoDesc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
	psoDesc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };
	psoDesc.BlendState = blendDesc_;
	psoDesc.RasterizerState = rasterizerDesc_;
	// 書き込むRTVの情報
	psoDesc.NumRenderTargets = kRenderTargetCount;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジのタイプ
	psoDesc.PrimitiveTopologyType = topo;
	// どのように画面に色を打ち込むかの設定
	psoDesc.SampleDesc.Count = kSampleDescCount;
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencil
	psoDesc.DepthStencilState = depthStencilDesc_;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 実際に生成
	result = dxBase_->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));
	assert(SUCCEEDED(result));
}
