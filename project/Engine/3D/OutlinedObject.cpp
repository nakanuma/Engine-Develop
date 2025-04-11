#include "OutlinedObject.h"

OutlinedObject::OutlinedObject()
{
	// アウトラインの設定
	outline_.materialCB_.data_->color = { 0.0f, 0.0f, 0.0f, 1.0f };
	outline_.materialCB_.data_->enableLighting = false;
}

void OutlinedObject::UpdateMatrix()
{
	// 本体のオブジェクト
	Object3D::UpdateMatrix();
	// 本体のトランスフォームをコピー
	outline_.transform_ = this->transform_;
	// スケールを変更
	outline_.transform_.scale.x *= kOutlineScale_;
	outline_.transform_.scale.y *= kOutlineScale_;
	outline_.transform_.scale.z *= kOutlineScale_;
	// 行列の更新
	outline_.UpdateMatrix();
}

void OutlinedObject::Draw()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 本体のオブジェクト
	Object3D::Draw();
	if (enableOutline) {
		// アウトラインのモデル情報を更新
		outline_.model_ = this->model_;
		// アウトライン用のPSOを設定
		dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineStateOutline());
		// アウトラインの描画
		outline_.Draw();
		// PSOを元に戻す
		dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineState());
	}
}
