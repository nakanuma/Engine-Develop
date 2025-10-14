#pragma once

// 前方宣言
class SceneManager;

/// <summary>
/// シーン基底クラス
/// </summary>
class BaseScene {
public:
	virtual ~BaseScene() = default;

	// 初期化
	virtual void Initialize();

	// 終了
	virtual void Finalize();

	// 毎フレーム更新
	virtual void Update();

	// 描画
	virtual void Draw();

public:
};
