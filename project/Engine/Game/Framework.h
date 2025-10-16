#pragma once
#include <Windows.h>
#include <assert.h>
#include <cstdint>
#include <list>
#include <numbers>

// MyClass
#include "AbstractSceneFactory.h"
#include "Camera.h"
#include "ConstBuffer.h"
#include "DescriptorHeap.h"
#include "DirectXBase.h"
#include "DirectXUtil.h"
#include "ImguiWrapper.h"
#include "Input.h"
#include "LineDrawer.h"
#include "Logger.h"
#include "ModelManager.h"
#include "MyMath.h"
#include "MyWindow.h"
#include "Object3D.h"
#include "SRVManager.h"
#include "SceneManager.h"
#include "ShadowMapManager.h"
#include "SoundManager.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "StringUtil.h"
#include "StructuredBuffer.h"
#include "TextureManager.h"
#include "TimeManager.h"

/// <summary>
/// アプリケーション・ゲームフレームワーク基盤クラス
/// </summary>
class Framework {
public:
	virtual ~Framework() = default;

	// 初期化
	virtual void Initialize();

	// 終了
	virtual void Finalize();

	// 毎フレーム更新
	virtual void Update();

	// 描画
	virtual void Draw() = 0;

	// 終了チェック
	virtual bool IsEndRequest() { return endRequest_; }

	// 終了リクエストを設定
	virtual void RequestEnd() { endRequest_ = true; }

	// 実行
	void Run();

protected:
	// 基盤機能
	Window* window = nullptr;
	DirectXBase* dxBase = nullptr;
	SRVManager* srvManager = nullptr;
	// 汎用機能
	SpriteCommon* spriteCommon = nullptr;
	SoundManager* soundManager = nullptr;
	// シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;

private:
	bool endRequest_;
};
