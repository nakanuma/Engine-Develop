#pragma once
#include <Windows.h>
#include <vector>
#include <wrl.h>

#include <XInput.h>
#define DIRECTINPUT_VERSON 0x0800 // DirectInputのバージョン指定
#include <dinput.h>

#include "MyWindow.h"

// 入力
class Input {
public:
	// namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	enum class PadType {
		DirectInput,
		XInput,
	};

	// variantがC++17から
	union State {
		XINPUT_STATE xInput_;
		DIJOYSTATE2 directInput_;
	};

	struct Joystick {
		ComPtr<IDirectInputDevice8> device_;
		int32_t deadZoneL_;
		int32_t deadZoneR_;
		PadType type_;
		State state_;
		State statePre_;
	};

public: // メンバ関数
	static Input* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Window* window);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNumber">キー番号（ DIK_0等 ）</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号（ DIK_0等 ）</param>
	/// <returns>トリガーか</returns>
	bool TriggerKey(BYTE keyNumber);

	/// <summary>
	/// キーのリリースをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号（ DIK_0等 ）</param>
	/// <returns>離されたか</returns>
	bool ReleaseKey(BYTE keyNumber);

	/// <summary>
	/// マウスの押下をチェック
	/// </summary>
	/// <param name="mouseNumber">マウスボタン番号(0:左, 1:右, 2:真ん中)</param>
	/// <returns>押されているか</returns>
	bool IsPressMouse(int32_t mouseNumber) const;

	/// <summary>
	/// マウスのトリガーをチェック。押した瞬間だけtrueになる
	/// </summary>
	/// <param name="mouseNumber">マウスボタン番号(0:左, 1:右, 2:真ん中)</param>
	/// <returns>トリガーか</returns>
	bool IsTriggerMouse(int32_t mouseNumber) const;

	/// <summary>
	/// マウス移動量を取得
	/// </summary>
	/// <returns></returns>
	POINT GetMouseMove() const;

	/// <summary>
	/// ホイールスクロール量を取得する
	/// </summary>
	/// <returns>奥側に回したら+</returns>
	int32_t GetWheel() const;

	/// <summary>
	/// マウスの位置を取得する（ウィンドウ座標系）
	/// </summary>
	const POINT& GetMousePosition() const;

	/// <summary>
	/// 現在のジョイスティック状態を取得する（XInput）
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号</param>
	/// <param name="out">現在のジョイスティック状態</param>
	/// <returns>正しく取得できたか</returns>
	bool GetJoystickState(int32_t stickNo, XINPUT_STATE& out) const;

	/// <summary>
	/// デッドゾーンを設定する
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号</param>
	/// <param name="deadZoneL">デッドゾーン左スティック</param>
	/// <param name="deadZoneR">デッドゾーン右スティック</param>
	void SetJoystickDeadZone(int32_t stickNo, int32_t deadZoneL, int32_t deadZoneR);

private: // メンバ変数
	// DirectInputのインスタンス
	ComPtr<IDirectInput8> directInput_;

	// キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard_;

	// マウスのデバイス
	ComPtr<IDirectInputDevice8> mouse_;

	// ジョイスティックのデバイス
	std::vector<Joystick> joysticks_;

	// 全キーの状態
	BYTE key_[256] = {};

	// 前回の全キーの状態
	BYTE keyPre[256] = {};

	// マウスの位置
	POINT mousePosition_;

	// マウスのボタン状態
	DIMOUSESTATE2 mouseState_ = {};
	DIMOUSESTATE2 mouseStatePre_ = {};

	// WindowsAPI
	Window* window = nullptr;
};