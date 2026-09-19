#include "camera.h"

// Engine
#include <DirectXBase.h>
#include <MyWindow.h>
#include <CommandManager.h>

Cygnus::Camera::Camera(const Float3& argTranslate, const Float3& argRotate, float argFov) {
	// 引数で受け取った位置、回転、視野角を設定
	transform_.translate_ = argTranslate;
	transform_.rotate_ = argRotate;
	transform_.scale_ = kDefaultScale;
	fov_ = argFov;

	// CBにカメラのポジションをセット
	cameraCB_.data_->position = argTranslate;
}

void Cygnus::Camera::TransferConstantBuffer() { 
	auto cmd = CommandManager::GetInstance()->GetCommandList();

	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexCamera, current_->cameraCB_.resource_->GetGPUVirtualAddress());
}                     

Cygnus::Matrix Cygnus::Camera::MakeViewMatrix() {
	// カメラのtransformからアフィン変換行列を作成
	Matrix affine = transform_.MakeAffineMatrix();
	// 逆行列を計算して返す（ビューマトリックス）
	return Matrix::Inverse(affine);
}

Cygnus::Matrix Cygnus::Camera::MakePerspectiveFovMatrix() {
	float aspectRatio;

	if(isCubeMapCamera_) {
		aspectRatio = 1.0f;
	} else {
		aspectRatio = static_cast<float>(Window::GetWidth()) / static_cast<float>(Window::GetHeight());
	}

	// 透視投影行列を生成して返す
	return Matrix::PerspectiveFovLH(
		fov_, 
		aspectRatio, 
		nearZ_, 
		farZ_
	);
}

Cygnus::Matrix Cygnus::Camera::MakeCubeMapPerspectiveFovMatrix()
{
	return Matrix::PerspectiveFovLH(
		PIf / 2.0f,
		1.0f,
		nearZ_,
		farZ_
	);
}
