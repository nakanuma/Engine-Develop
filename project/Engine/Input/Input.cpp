#include "Input.h"
#include <cassert>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#pragma comment(lib, "xinput.lib")

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

void Input::Initialize(Window* window) {
	// 借りてきたwinAppのインスタンスを記録
	this->window = window;

	// DirectInputの初期化
	HRESULT result = DirectInput8Create(window->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
	assert(SUCCEEDED(result));

	// キーボードデバイスの生成
	result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboard_->SetCooperativeLevel(window->GetHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	// マウスデバイスの生成
	result = directInput_->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = mouse_->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = mouse_->SetCooperativeLevel(window->GetHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	// XInputデバイスの登録
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++) {
		XINPUT_STATE state = {};
		if (XInputGetState(i, &state) == ERROR_SUCCESS) {
			// XInputデバイスが接続されている場合
			Joystick joystick;
			joystick.type_ = PadType::XInput;
			joystick.deadZoneL_ = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
			joystick.deadZoneR_ = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
			joysticks_.push_back(joystick);
		}
	}
}

void Input::Update() {
	HRESULT result;

	// 前回のキー入力を保存
	memcpy(keyPre, key_, sizeof(key_));

	// キーボード情報の取得開始
	result = keyboard_->Acquire();
	// 全キーの入力状態を取得する
	result = keyboard_->GetDeviceState(sizeof(key_), key_);

	// マウス情報の取得開始
	result = mouse_->Acquire();

	// 前回のマウス状態を保存
	mouseStatePre_ = mouseState_;

	// マウスの状態を更新
	mouse_->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState_);

	// マウス位置の更新
	GetCursorPos(&mousePosition_);
	ScreenToClient(window->GetHandle(), &mousePosition_);
}

bool Input::PushKey(BYTE keyNumber) {
	// 指定キーを押していればtrueを返す
	if (key_[keyNumber]) {
		return true;
	}
	// そうでなければfalseを返す
	return false;
}

bool Input::TriggerKey(BYTE keyNumber) {
	// 指定キーが押された瞬間のみtrueを返す
	if (key_[keyNumber] && !keyPre[keyNumber]) {
		return true;
	}
	// そうでなければfalseを返す
	return false;
}

bool Input::ReleaseKey(BYTE keyNumber) {
	// 指定キーが離された瞬間のみtrueを返す
	if (!key_[keyNumber] && keyPre[keyNumber]) {
		return true;
	}
	// そうでなければfalseを返す
	return false;
}

bool Input::IsPressMouse(int32_t mouseNumber) const {
	// ボタン番号の範囲チェック
	if (mouseNumber < 0 || mouseNumber > 3) {
		return false;
	}

	return (mouseState_.rgbButtons[mouseNumber] & 0x80) != 0;
}

bool Input::IsTriggerMouse(int32_t mouseNumber) const {
	// ボタン番号の範囲チェック
	if (mouseNumber < 0 || mouseNumber > 3) {
		return false;
	}

	// 前フレームで押されていないかつ、現在押されている場合のみtrueを返す
	return !(mouseStatePre_.rgbButtons[mouseNumber] & 0x80) && (mouseState_.rgbButtons[mouseNumber] & 0x80);
}

POINT Input::GetMouseMove() const {
	POINT move = {mouseState_.lX, mouseState_.lY};
	return move;
}

int32_t Input::GetWheel() const { return mouseState_.lZ; }

const POINT& Input::GetMousePosition() const { return mousePosition_; }

bool Input::GetJoystickState(int32_t stickNo, XINPUT_STATE& out) const {
	// stickNoが範囲内かチェック
	if (stickNo < 0 || stickNo >= joysticks_.size()) {
		return false; // 無効なジョイスティック番号
	}

	// 指定されたジョイスティックを取得
	const Joystick& joystick = joysticks_[stickNo];

	// ジョイスティックがXInputタイプか確認
	if (joystick.type_ != PadType::XInput) {
		return false; // DirectInputコントローラーの場合は処理しない
	}

	// XInputから状態を取得
	DWORD result = XInputGetState(stickNo, &out);
	if (result != ERROR_SUCCESS) {
		return false; // 状態取得失敗
	}

	///
	///	デッドゾーンの適用
	///

	// 左スティック
	if (abs(out.Gamepad.sThumbLX) < joysticks_[stickNo].deadZoneL_) {
		out.Gamepad.sThumbLX = 0;
	}
	if (abs(out.Gamepad.sThumbLY) < joysticks_[stickNo].deadZoneL_) {
		out.Gamepad.sThumbLY = 0;
	}
	// 右スティック
	if (abs(out.Gamepad.sThumbRX) < joysticks_[stickNo].deadZoneL_) {
		out.Gamepad.sThumbRX = 0;
	}
	if (abs(out.Gamepad.sThumbRY) < joysticks_[stickNo].deadZoneL_) {
		out.Gamepad.sThumbRY = 0;
	}

	return true; // 正常に取得
}

void Input::SetJoystickDeadZone(int32_t stickNo, int32_t deadZoneL, int32_t deadZoneR) {
	// ジョイスティック番号が有効か確認
	if (stickNo < 0 || stickNo >= joysticks_.size()) {
		return; // 無効なジョイスティック番号の場合は終了
	}

	// 対象のジョイスティックを取得
	Joystick& joystick = joysticks_[stickNo];

	// 左右スティックのデッドゾーンを設定
	joystick.deadZoneL_ = deadZoneL;
	joystick.deadZoneR_ = deadZoneR;
}