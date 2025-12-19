#include "DirectXBase.h" 

// C++
#include <cassert>

// Engine
#include <Logger.h>
#include <StringUtil.h>
#include <DirectXUtil.h>
#include <RTVManager.h> 
#include <PipelineStateManager.h>
#include <RootSignatureManager.h>
#include <FrameResourceManager.h>

Cygnus::DirectXBase::~DirectXBase()
{
	CloseHandle(fenceEvent_);

	Log("Released DirectXBase\n");
}

Cygnus::DirectXBase* Cygnus::DirectXBase::GetInstance()
{
	static DirectXBase instance;
	return &instance;
}

void Cygnus::DirectXBase::Initialize() {
	// FPS固定初期化
	FPSController::GetInstance()->InitializeFixFPS();
	// DXGIデバイス初期化
	InitializeDXGIDevice();
	// コマンド関連初期化
	InitializeCommand();
	// スワップチェーンの生成
	CreateSwapChain();
	// FrameResourceManagerの初期化
	FrameResourceManager::GetInstance()->Initialize(device_.Get(), swapChain_.Get());
	// フェンス生成
	CreateFence();
	// InputLayoutの設定
	SetInputLayout();
	// BlendStateの設定
	SetBlendState();
	SetBlendStateNone();
	SetBlendStateAdd();
	SetBlendStateSubtract();
	SetBlendStateMultiply();
	SetBlendStateScreen();
	SetBlendStateAlpha();
	// RasterizerStateの設定
	SetRasterizerState();
	// ShaderManagerの初期化
	ShaderManager::GetInstance()->Initialize();
	// RootSignatureManagerの初期化
	RootSignatureManager::GetInstance()->Initialize(this->GetDevice());

	depthStencilDesc_.DepthEnable = true;
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// PipelineStateManagerの初期化
	PipelineStateManager::GetInstance()->Initialize(
		this->GetDevice(),
		inputLayoutDesc_,
		blendDesc_,
		blendDescNone_,
		blendDescAdd_,
		blendDescSubtract_,
		blendDescMultiply_,
		blendDescScreen_,
		blendDescAlpha_,
		rasterizerDesc_,
		depthStencilDesc_
	);
	// Viewportの設定
	SetViewport();
	// Scissorの設定
	SetScissor();
}

void Cygnus::DirectXBase::InitializeDXGIDevice([[maybe_unused]]bool enableDebugLayer)
{
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	HRESULT result = S_FALSE;

	// DXGIファクトリーの生成
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result));

	// 使用するアダプタを決定する
	useAdapter_ = nullptr;
	// 良い順にアダプタを積む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_))
		!= DXGI_ERROR_NOT_FOUND; ++i) {
		// アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		result = useAdapter_->GetDesc3(&adapterDesc);
		// 取得できたか判定
		assert(SUCCEEDED(result));
		// ソフトウェアでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// 採用したアダプタの情報をログに出力
			Log(std::format(L"Use Adapter:{}\n", adapterDesc.Description));
			break;
		}
		useAdapter_ = nullptr; // ソフトウェアアダプタの場合は見なかったことにする
	}
	// 適切なアダプタが見つからなかったので起動できない
	assert(useAdapter_ != nullptr);

	// D3D12Deviceの生成
	device_ = nullptr;
	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
	// 高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		// 採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		// 指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(result)) {
			// 生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	// デバイスの生成がうまくいかなかったので起動できない
	assert(device_ != nullptr);
	Log("Complete create D3D12Device!!!\n"); // 初期化完了のログを出す

#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// ヤバイエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			// Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
#endif
}

void Cygnus::DirectXBase::InitializeCommand()
{
	HRESULT result = S_FALSE;

	// コマンドキューを生成する
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	result = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(result));

	// コマンドアロケータを生成する
	commandAllocator_ = nullptr;
	result = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(result));

	// コマンドリストを生成する
	commandList_ = nullptr;
	result = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(result));
}

void Cygnus::DirectXBase::CreateSwapChain()
{
	HRESULT result = S_FALSE;

	// スワップチェーンを生成する
	swapChain_ = nullptr;
	swapChainDesc_.Width = Window::GetWidth(); // 画面の幅(クライアント領域と同じにする)
	swapChainDesc_.Height = Window::GetHeight(); // 画面の高さ(クライアント領域と同じにする)
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色の形式
	swapChainDesc_.SampleDesc.Count = 1; // マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画のターゲットとして利用する
	swapChainDesc_.BufferCount = 2; // ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // モニタに写したら中身を破棄
	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	result = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), Window::GetHandle(), &swapChainDesc_,
		nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(result));
}

void Cygnus::DirectXBase::CreateFence()
{
	HRESULT result = S_FALSE;

	// 初期値0でFenceを作る
	fence_ = nullptr;
	fenceValue_ = 0;
	result = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));

	// FenceのSignalを待つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}

void Cygnus::DirectXBase::SetInputLayout()
{
	// InputLayout
	inputElementDescs_[kInputElementIndexPositon].SemanticName = "POSITION";
	inputElementDescs_[kInputElementIndexPositon].SemanticIndex = 0;
	inputElementDescs_[kInputElementIndexPositon].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[kInputElementIndexPositon].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[kInputElementIndexTexcoord].SemanticName = "TEXCOORD";
	inputElementDescs_[kInputElementIndexTexcoord].SemanticIndex = 0;
	inputElementDescs_[kInputElementIndexTexcoord].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs_[kInputElementIndexTexcoord].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[kInputElementIndexNormal].SemanticName = "NORMAL";
	inputElementDescs_[kInputElementIndexNormal].SemanticIndex = 0;
	inputElementDescs_[kInputElementIndexNormal].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[kInputElementIndexNormal].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[kInputElementIndexWeight].SemanticName = "WEIGHT";
	inputElementDescs_[kInputElementIndexWeight].SemanticIndex = 0;
	inputElementDescs_[kInputElementIndexWeight].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // float32_t4
	inputElementDescs_[kInputElementIndexWeight].InputSlot = 1; // 1番目のslotのVBVのことだと伝える
	inputElementDescs_[kInputElementIndexWeight].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[kInputElementIndex].SemanticName = "INDEX";
	inputElementDescs_[kInputElementIndex].SemanticIndex = 0;
	inputElementDescs_[kInputElementIndex].Format = DXGI_FORMAT_R32G32B32A32_SINT; // int32_t4
	inputElementDescs_[kInputElementIndex].InputSlot = 1; // 1番目のslotのVBVのことだと伝える
	inputElementDescs_[kInputElementIndex].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);
}

D3D12_BLEND_DESC Cygnus::DirectXBase::SetBlendState()
{
	///
	///	kBlendModeNormal
	/// 

	// BlendStateの設定
	// すべての色要素を書き込む
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

D3D12_BLEND_DESC Cygnus::DirectXBase::SetBlendStateNone()
{
	blendDescNone_.RenderTarget[kRenderTargetIndex].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return blendDescNone_;
}

D3D12_BLEND_DESC Cygnus::DirectXBase::SetBlendStateAdd()
{
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

D3D12_BLEND_DESC Cygnus::DirectXBase::SetBlendStateSubtract()
{
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

D3D12_BLEND_DESC Cygnus::DirectXBase::SetBlendStateMultiply()
{
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

D3D12_BLEND_DESC Cygnus::DirectXBase::SetBlendStateScreen()
{
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

D3D12_BLEND_DESC Cygnus::DirectXBase::SetBlendStateAlpha()
{
	blendDescAlpha_.RenderTarget[kRenderTargetIndex].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDescAlpha_.RenderTarget[kRenderTargetIndex].BlendEnable = TRUE;
	blendDescAlpha_.RenderTarget[kRenderTargetIndex].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDescAlpha_.RenderTarget[kRenderTargetIndex].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescAlpha_.RenderTarget[kRenderTargetIndex].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDescAlpha_.RenderTarget[kRenderTargetIndex].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescAlpha_.RenderTarget[kRenderTargetIndex].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescAlpha_.RenderTarget[kRenderTargetIndex].DestBlendAlpha = D3D12_BLEND_ZERO;

	return blendDescAlpha_;
}

D3D12_RASTERIZER_DESC Cygnus::DirectXBase::SetRasterizerState()
{
	// 裏面（時計回り）を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

	return rasterizerDesc_;
}

void Cygnus::DirectXBase::SetViewport()
{
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = static_cast<float>(Window::GetWidth());
	viewport_.Height = static_cast<float>(Window::GetHeight());
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = kViewportMinDepth;
	viewport_.MaxDepth = kViewportMaxDepth;
}

void Cygnus::DirectXBase::SetScissor()
{
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = Window::GetWidth();
	scissorRect_.top = 0;
	scissorRect_.bottom = Window::GetHeight();
}

void Cygnus::DirectXBase::BeginFrame()
{
	// フレーム開始処理
	FrameResourceManager::GetInstance()->BeginFrame(commandList_.Get());
}

void Cygnus::DirectXBase::EndFrame()
{
	HRESULT result = S_FALSE;

	// GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);

	// Fenceの値を更新
	fenceValue_++;
	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);

	// Fenceの値が指定したSignal値にたどり着いているか確認する
	// GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_) {
		// 指定したSignalにたどりついていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		// イベント待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	// 次のフレーム用のコマンドリストを準備
	result = commandAllocator_->Reset();
	assert(SUCCEEDED(result));
	result = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));
}

void Cygnus::DirectXBase::PreDraw()
{
	// 描画に必要な情報をコマンドリストに積む
	commandList_->RSSetViewports(1, &viewport_); // Viewportを設定
	commandList_->RSSetScissorRects(1, &scissorRect_); // Scirssorを設定
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(RootSignatureManager::GetInstance()->GetRootSignature(RootSignatureType::Default));
	commandList_->SetPipelineState(PipelineStateManager::GetInstance()->GetPSO(PSOType::Default)); // PSOを設定
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておければ良い
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Cygnus::DirectXBase::PostDraw()
{
	HRESULT result = S_FALSE;

	// 画面に書く処理はすべて終わり、画面に映すので、状態を遷移
	RTVManager::ResetResourceBarrier();

	// コマンドリストの内容を確定させる
	result = commandList_->Close();
	assert(SUCCEEDED(result));

	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
	
	// FPS固定
	FPSController::GetInstance()->UpdateFixFPS();
}

ID3D12Device* Cygnus::DirectXBase::GetDevice()
{
	return device_.Get();
}

ID3D12GraphicsCommandList* Cygnus::DirectXBase::GetCommandList()
{
	return commandList_.Get();
}

IDXGISwapChain4* Cygnus::DirectXBase::GetSwapChain() {
	return swapChain_.Get(); 
}

DXGI_SWAP_CHAIN_DESC1 Cygnus::DirectXBase::GetSwapChainDesc() {
	return swapChainDesc_; 
}

Cygnus::D3DResourceLeakChecker::~D3DResourceLeakChecker()
{
	Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		// デバッグ用メッセージ出力
		Log("Reporting LiveObjects:\n");
		// 全てのLiveObjectを報告
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		// アプリケーション側で管理しているリソースを報告
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		// D3D12オブジェクトのリークを報告
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	}
}
