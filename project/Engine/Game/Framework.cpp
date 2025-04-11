#include "Framework.h"

void Framework::Initialize()
{
	// リークチェッカー
	D3DResourceLeakChecker::GetInstance();
	// COMの初期化
	CoInitializeEx(0,COINIT_MULTITHREADED);

	// ゲームウィンドウの生成
	window = new Window;
	window->Create(L"CG2WindowClass",1280,720);

	// DirectX初期化処理
	dxBase = DirectXBase::GetInstance();
	dxBase->Initialize();

	// SRVマネージャの初期化
	srvManager = SRVManager::GetInstance();
	srvManager->Initialize(dxBase);

	// 入力デバイスの生成と初期化
	Input::GetInstance()->Initialize(window);

	// スプライト共通部の初期化
	spriteCommon = new SpriteCommon;
	spriteCommon->Initialize(dxBase);

	// TextureManagerの初期化
	TextureManager::Initialize(dxBase->GetDevice(),srvManager);

	// ImGuiの初期化
	ImguiWrapper::Initialize(dxBase->GetDevice(),dxBase->GetSwapChainDesc().BufferCount,dxBase->GetRtvDesc().Format,srvManager->descriptorHeap.heap_.Get());

	// ParticleManagerの生成と初期化
	particleManager = new ParticleManager;
	particleManager->Initialize(dxBase,srvManager);

	// SoundManagerの生成と初期化
	soundManager = new SoundManager;
	soundManager->Initialize();

	GlobalVariables::getInstance()->LoadAllFile();
}

void Framework::Finalize()
{
	// シーンファクトリ開放
	delete sceneFactory_;
	// ParticleManager開放
	delete particleManager;
	// スプライト共通処理開放
	delete spriteCommon;
	// SoundManager開放
	delete soundManager;

	// ImGuiの終了処理
	ImguiWrapper::Finalize();
	// COMの終了処理
	CoUninitialize();
}

void Framework::Update()
{
	// ウィンドウのメッセージを処理して終了リクエストを設定
	if(window->ProcessMessage())
	{
		RequestEnd();
		return;
	}

	// 入力の更新
	Input::GetInstance()->Update();
	// フレーム開始処理
	dxBase->BeginFrame();
	// パーティクルマネージャの更新
	particleManager->Update();



	// SceneManagerの更新
	SceneManager::GetInstance()->Update();
}

void Framework::Run()
{
	// ゲームの初期化
	Initialize();

	while(true) // ゲームループ
	{
		// 毎フレーム更新
		Update();
		// 終了リクエストが来たら抜ける
		if(IsEndRequest())
		{
			break;
		}
		// 描画
		Draw();
	}
	// ゲームの終了
	Finalize();
}
