#include "Framework.h"

// Engine
#include <StringUtil.h>
#include <FrameResourceManager.h>

void Cygnus::Framework::Initialize() {
	// リークチェッカー
	D3DResourceLeakChecker::GetInstance();
	// COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	// ゲームウィンドウの生成
	window_ = std::make_unique<Window>();
	window_->Create(L"Application", kWindowWidth, kWindowHeight);

	// DirectX初期化処理
	dxBase_ = DirectXBase::GetInstance();
	dxBase_->Initialize();

	// SRVマネージャの初期化
	srvManager_ = SRVManager::GetInstance();
	srvManager_->Initialize(dxBase_);

	// 入力デバイスの生成と初期化
	Input::GetInstance()->Initialize(window_.get());

	// スプライト共通部の初期化
	spriteCommon_ = std::make_unique<Cygnus::SpriteCommon>();
	spriteCommon_->Initialize(dxBase_);

	// TextureManagerの初期化
	TextureManager::Initialize(dxBase_->GetDevice(), srvManager_);

	// ImGuiの初期化
	ImguiWrapper::Initialize(
		dxBase_->GetDevice(), 
		dxBase_->GetSwapChainDesc().BufferCount, 
		FrameResourceManager::GetInstance()->GetRTVDesc().Format, 
		srvManager_->descriptorHeap_.heap_.Get()
	);

	// SoundManagerの初期化
	SoundManager::GetInstance()->Initialize();

	// LineDrawer初期化
	LineDrawer::GetInstance()->Initialize();

	// ShadowMapManager初期化
	ShadowMapManager::GetInstance()->Initialize();
}

void Cygnus::Framework::Finalize() {
	// ImGuiの終了処理
	ImguiWrapper::Finalize();
	// COMの終了処理
	CoUninitialize();
}

void Cygnus::Framework::Update() {
	// ウィンドウのメッセージを処理して終了リクエストを設定
	if (window_->ProcessMessage()) {
		RequestEnd();
		return;
	}

	// 入力の更新
	Input::GetInstance()->Update();
	// フレーム開始処理
	dxBase_->BeginFrame();

	// SceneManagerの更新
	SceneManager::GetInstance()->Update();
	// TimeManagerの更新
	TimeManager::GetInstance()->Update();
}

void Cygnus::Framework::Run() {
	// ゲームの初期化
	Initialize();

	while (true) // ゲームループ
	{
		// 毎フレーム更新
		Update();
		// 終了リクエストが来たら抜ける
		if (IsEndRequest()) {
			break;
		}
		// 描画
		Draw();
	}
	// ゲームの終了
	Finalize();
}
