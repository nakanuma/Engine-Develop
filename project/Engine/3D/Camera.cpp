#include "camera.h"
#include "MyWindow.h"
#include "DirectXBase.h"

Camera::Camera(Float3 argTranslate, Float3 argRotate, float argFov)
{
	// 引数で受け取った位置、回転、視野角を設定
	transform.translate = argTranslate;
	transform.rotate = argRotate;
	transform.scale = { 1.0f, 1.0f, 1.0 };
	fov = argFov;

	// CBにカメラのポジションをセット
	cameraCB_.data_->position = argTranslate;
}

void Camera::TransferConstantBuffer()
{
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, current_->cameraCB_.resource_->GetGPUVirtualAddress());
}

Matrix Camera::MakeViewMatrix()
{
	// カメラのtransformからアフィン変換行列を作成
	Matrix affine = transform.MakeAffineMatrix();
	// 逆行列を計算して返す（ビューマトリックス）
	return Matrix::Inverse(affine);
}

Matrix Camera::MakePerspectiveFovMatrix()
{
	// 透視投影行列を生成して返す
	return Matrix::PerspectiveFovLH(fov, static_cast<float>(Window::GetWidth()) / static_cast<float>(Window::GetHeight()), nearZ, farZ);
}
