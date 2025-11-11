#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <Windows.h>
#include <assert.h>
#include <cstdint>
#include <list>
#include <numbers>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <AbstractSceneFactory.h>
#include <Camera.h>
#include <ConstBuffer.h>
#include <DescriptorHeap.h>
#include <DirectXBase.h>
#include <DirectXUtil.h>
#include <ImguiWrapper.h>
#include <Input.h>
#include <LineDrawer.h>
#include <Logger.h>
#include <ModelManager.h>
#include <MyMath.h>
#include <MyWindow.h>
#include <Object3D.h>
#include <SRVManager.h>
#include <SceneManager.h>
#include <ShadowMapManager.h>
#include <SoundManager.h>
#include <Sprite.h>
#include <SpriteCommon.h>
#include <StringUtil.h>
#include <StructuredBuffer.h>
#include <TextureManager.h>
#include <TimeManager.h>

// =========================================================
// アプリケーション・ゲームフレームワーク基盤クラス
// =========================================================
class Framework {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Framework() = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// 毎フレーム更新処理
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 終了リクエストの有無を確認
	/// </summary>
	/// <returns>終了リクエスト</returns>
	virtual bool IsEndRequest() { return endRequest_; }

	/// <summary>
	/// 終了リクエストを出す
	/// </summary>
	virtual void RequestEnd() { endRequest_ = true; }

	/// <summary>
	/// メインループ
	/// </summary>
	void Run();

protected:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- System -----
	Window* window = nullptr;							/* ウィンドウクラス */
	DirectXBase* dxBase = nullptr;						/* DirectX基盤クラス */
	SRVManager* srvManager = nullptr;					/* SRV管理クラス */
	SpriteCommon* spriteCommon = nullptr;				/* スプライト共通処理クラス */
	SoundManager* soundManager = nullptr;				/* サウンド管理クラス */
	AbstractSceneFactory* sceneFactory_ = nullptr;		/* シーンファクトリクラス */

private:
	bool endRequest_;									/* 終了リクエストフラグ */
};