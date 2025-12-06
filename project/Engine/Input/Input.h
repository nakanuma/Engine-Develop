#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <Windows.h>
#include <vector>
#include <wrl.h>
#include <XInput.h>
#define DIRECTINPUT_VERSON 0x0800 // DirectInputのバージョン指定
#include <dinput.h>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include "MyWindow.h"

namespace Cygnus {
// =========================================================
// 入力管理クラス
// =========================================================
class Input {
public:
	// namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	/// <summary>
	/// ゲームパッドの種類
	/// </summary>
	enum class PadType {
		DirectInput,					/* DirectInput */
		XInput,							/* XInput */
	};

	/// <summary>
	/// ゲームパッドの状態
	/// </summary>
	union State {
		XINPUT_STATE xInput;			/* XInput用 */
		DIJOYSTATE2 directInput;		/* DirectInput用 */
	};

	/// <summary>
	/// ゲームパッド入力を管理する構造体
	/// </summary>
	struct Joystick {
		ComPtr<IDirectInputDevice8> device;		/* DirectInputデバイス */
		int32_t deadZoneL;						/* デッドゾーン左スティック */
		int32_t deadZoneR;						/* デッドゾーン右スティック */
		PadType type;							/* ゲームパッドの種類 */
		State state;							/* 現在の状態 */
		State statePre;							/* 前回の状態 */
	};

public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static Input* GetInstance();

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	/// <param name="window"></param>
	void Initialize(Window* window);

	/// <summary>
	/// 毎フレームの更新処理を行います。
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

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr size_t kKeyArraySize = 256;	/* キーボード状態配列のサイズ */

	static constexpr BYTE kMouseButtonPressMask = 0x80;	/* マウスボタンの状態チェックに使用するビットマスク */
	static constexpr int32_t kMouseIndexMin = 0;	/* マウスボタンの最小インデックス */
	static constexpr int32_t kMouseIndexMax = 3;	/* マウスボタンの最大インデックス */

	// =========================================================
	// Member Variables
	// =========================================================
	
	ComPtr<IDirectInput8> directInput_;			/* DirectInputインターフェース */
	ComPtr<IDirectInputDevice8> keyboard_;		/* キーボードデバイス */
	ComPtr<IDirectInputDevice8> mouse_;			/* マウスデバイス */

	std::vector<Joystick> joysticks_;			/* ジョイスティックデバイス */

	BYTE key_[kKeyArraySize] = {};				/* 現在のキー状態 */
	BYTE keyPre_[kKeyArraySize] = {};			/* 前回のキー状態 */

	POINT mousePosition_;						/* マウスの位置 */
	DIMOUSESTATE2 mouseState_ = {};				/* 現在のマウス状態 */
	DIMOUSESTATE2 mouseStatePre_ = {};			/* 前回のマウス状態 */

	Window* window_ = nullptr;					/* ウィンドウクラス */
};
}