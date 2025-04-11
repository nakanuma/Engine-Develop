#pragma once
#include <Windows.h> 
#include <cstdint>
#include <assert.h>
#include <numbers>
#include <list>

// MyClass 
#include "MyWindow.h"
#include "Logger.h"
#include "StringUtil.h"
#include "DirectXBase.h"
#include "DirectXUtil.h"
#include "MyMath.h"
#include "Camera.h"
#include "DescriptorHeap.h"
#include "ImguiWrapper.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ConstBuffer.h"
#include "Object3D.h"
#include "OutlinedObject.h"
#include "Input.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "SRVManager.h"
#include "StructuredBuffer.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include "SoundManager.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"

#include "GlobalVariables.h"

class Framework
{
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
	ParticleManager* particleManager = nullptr;
	SoundManager* soundManager = nullptr;
	// シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;


private:
	bool endRequest_;
};

