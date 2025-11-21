#include "camera.h"
#include "DirectXBase.h"
#include "MyWindow.h"

Camera::Camera(Float3 argTranslate, Float3 argRotate, float argFov) {
	// 引数で受け取った位置、回転、視野角を設定
	transform_.translate_ = argTranslate;
	transform_.rotate_ = argRotate;
	transform_.scale_ = kDefaultScale;
	fov_ = argFov;

	// CBにカメラのポジションをセット
	cameraCB_.data_->position = argTranslate;
}

void Camera::TransferConstantBuffer() { DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(kRootParameterIndexCamera, current_->cameraCB_.resource_->GetGPUVirtualAddress()); }

Matrix Camera::MakeViewMatrix() {
	// カメラのtransformからアフィン変換行列を作成
	Matrix affine = transform_.MakeAffineMatrix();
	// 逆行列を計算して返す（ビューマトリックス）
	return Matrix::Inverse(affine);
}

Matrix Camera::MakePerspectiveFovMatrix() {
	// 透視投影行列を生成して返す
	return Matrix::PerspectiveFovLH(fov_, static_cast<float>(Window::GetWidth()) / static_cast<float>(Window::GetHeight()), nearZ_, farZ_);
}
