#include "DebugCamera.h"

void DebugCamera::Initialize() {
	// transformの初期化
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.translate = {0.0f, 0.0f, -50.0f};
	transform_.scale = {1.0f, 1.0f, 1.0f};

	// 各行列を単位行列で埋める
	viewMatrix_ = Matrix::Identity();
	projectionMatrix_ = Matrix::Identity();

	rotateMatrix_ = Matrix::Identity();
	translateMatrix_ = Matrix::Identity();
	WorldMatrix_ = Matrix::Identity();
}

void DebugCamera::Update(Input* input) {
	///
	///	入力によるカメラの移動や回転
	///

	const float kInertiaFactor = 0.8f; // 0.0f ~ 1.0fの間

	// カメラの向きに基づいた前方向ベクトルを取得
	Float3 forward = Float3(-std::sin(transform_.rotate.y), std::sin(transform_.rotate.x), -std::cos(transform_.rotate.y));
	// カメラの向きに基づいた右方向ベクトルを取得
	Float3 right = Float3(std::cos(transform_.rotate.y), 0.0f, -std::sin(transform_.rotate.y));

	// 移動方向と速度を保持する変数
	static Float3 velocity(0.0f, 0.0f, 0.0f);

	/* 右クリック長押し + WASDキーで上下左右の移動 */
	if (input->IsPressMouse(1)) {
		// 加速度を基に移動
		float acceleration = moveSpeed_;

		// 上
		if (input->PushKey(DIK_W)) {
			velocity -= forward * acceleration;
		}
		// 下
		if (input->PushKey(DIK_S)) {
			velocity += forward * moveSpeed_;
		}

		// 右
		if (input->PushKey(DIK_D)) {
			velocity += right * moveSpeed_;
		}
		// 左
		if (input->PushKey(DIK_A)) {
			velocity -= right * moveSpeed_;
		}
	}

	// 速度を減衰させる
	transform_.translate += velocity;
	velocity *= kInertiaFactor;

	/* 右クリック長押し + ドラッグで回転 */
	if (input->IsPressMouse(1)) {
		// マウスの移動量を取得
		int32_t dx, dy;
		dx = input->GetMouseMove().x;
		dy = input->GetMouseMove().y;

		transform_.rotate.x += static_cast<float>(dy) * 0.001f;
		transform_.rotate.y += static_cast<float>(dx) * 0.001f;
	}

	///
	///	ビュー行列の更新
	///

	// 角度から回転行列を計算
	rotateMatrix_ = Matrix::RotationRollPitchYaw(transform_.rotate.x, transform_.rotate.y, transform_.rotate.z);
	// 座標から平行移動行列を計算
	translateMatrix_ = Matrix::Translation(transform_.translate);
	// 回転行列と平行移動行列からワールド行列を計算する
	WorldMatrix_ = rotateMatrix_ * translateMatrix_;
	// ワールド行列の逆行列をビュー行列に代入する
	viewMatrix_ = Matrix::Inverse(WorldMatrix_);
}