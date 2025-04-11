#include "Input.h"
#include <cassert>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Input* Input::GetInstance()
{
	static Input instance;
	return &instance;
}

void Input::Initialize(Window* window)
{
	// 借りてきたwinAppのインスタンスを記録
	this->window = window;

	// DirectInputの初期化
	HRESULT result = DirectInput8Create(
		window->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(result));

	// キーボードデバイスの生成
	result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboard_->SetCooperativeLevel(
		window->GetHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));


	// マウスデバイスの生成
	result = directInput_->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = mouse_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = mouse_->SetCooperativeLevel(
		window->GetHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update()
{
	HRESULT result;

	// 前回のキー入力を保存
	memcpy(keyPre, key_, sizeof(key_));

	// キーボード情報の取得開始
	result = keyboard_->Acquire();
	// 全キーの入力状態を取得する
	result = keyboard_->GetDeviceState(sizeof(key_), key_);


	// マウス情報の取得開始
	result = mouse_->Acquire();

	// マウスの状態を更新
	DIMOUSESTATE mouseState;
	mouse_->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);

	// マウス位置の更新
	GetCursorPos(&mousePosition_);
	ScreenToClient(window->GetHandle(), &mousePosition_);

	// 前回のマウスボタン状態を保存
	memcpy(mouseButtonsPre_, mouseButtons_, sizeof(mouseButtonsPre_));
	// マウスボタンの更新
	memcpy(mouseButtons_, mouseState.rgbButtons, sizeof(mouseButtons_));
}

bool Input::PushKey(BYTE keyNumber)
{
	// 指定キーを押していればtrueを返す
	if (key_[keyNumber]) {
		return true;
	}
	// そうでなければfalseを返す
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	// 指定キーが押された瞬間のみtrueを返す
	if (key_[keyNumber] && !keyPre[keyNumber]) {
		return true;
	}
	// そうでなければfalseを返す
	return false;
}

bool Input::ReleaseKey(BYTE keyNumber)
{
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

	return (mouseButtons_[mouseNumber] & 0x80) != 0;
}

bool Input::IsTriggerMouse(int32_t mouseNumber) const { 
	// ボタン番号の範囲チェック
	if (mouseNumber < 0 || mouseNumber > 3) {
		return false;
	}

	// 前フレームで押されていないかつ、現在押されている場合のみtrueを返す
	return !(mouseButtonsPre_[mouseNumber] & 0x80) && (mouseButtons_[mouseNumber] & 0x80);
}

const POINT& Input::GetMousePosition() const { 
	return mousePosition_; 
}
