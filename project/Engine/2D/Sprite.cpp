#include "Sprite.h"
#include "DirectXUtil.h"
#include "SpriteCommon.h"
#include "TextureManager.h"

void Sprite::Initialize(SpriteCommon* spriteCommon, uint32_t textureIndex) {
	// 引数で受け取ってメンバ変数に記録する
	this->spriteCommon_ = spriteCommon;
	// テクスチャを保存
	textureIndex_ = textureIndex;

	// VertexResourceを作る
	vertexResource_ = CreateBufferResource(spriteCommon->GetDxBase()->GetDevice(), sizeof(VertexData) * kVertexCount);
	// IndexResourceを作る
	indexResource_ = CreateBufferResource(spriteCommon->GetDxBase()->GetDevice(), sizeof(uint32_t) * kIndexCount);
	// materialResourceを作る
	materialResource_ = CreateBufferResource(spriteCommon->GetDxBase()->GetDevice(), sizeof(Material));
	// TransformationResourceを作る
	transformationMatrixResource_ = CreateBufferResource(spriteCommon->GetDxBase()->GetDevice(), sizeof(TransformationMatrix));

	// VertexBufferViewを作成する
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * kVertexCount;
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	// IndexBufferViewを作成する
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * kIndexCount;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	// VertexResourceにデータを書き込むためのアドレスを取得してvertexDataに割り当てる
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	// IndexResourceにデータを書き込むためのアドレスを取得してindexDataに割り当てる
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	// materialResourceにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// materialDataの初期値を書き込む
	materialData_->color = kDefaultColor;
	materialData_->enableLighting = false;
	materialData_->uvTransform = Matrix::Identity();
	materialData_->shininess = kDefaultShininess;
	materialData_->ratio = kDefaultRatio;
	materialData_->useCircleMask = false;

	// TransformationResourceにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
	// 単位行列を書き込んでおく
	transformationMatrixData_->WVP = Matrix::Identity();
	transformationMatrixData_->World = Matrix::Identity();

	// Transformの初期化
	transform_ = {
		kDefaultTranslation,
		kDefaultRotation,
		kDefaultScale
	};

	// テクスチャの解像度を取得して、スプライト自体のサイズに反映させる
	AdjustTextureSize();
}

void Sprite::Update() {
	// 座標を反映
	transform_.translate_ = { position_.x, position_.y, kDefaultTranslation.z };
	// 回転を反映
	transform_.rotate_ = { kDefaultRotation.x, kDefaultRotation.y, rotation };
	// サイズを反映
	transform_.scale_ = { size_.x, size_.y, kDefaultScale.z };
	// アンカーポイント
	float left = kAnchorLeft - anchorPoint.x;
	float right = kAnchorRight - anchorPoint.x;
	float top = kAnchorTop - anchorPoint.y;
	float bottom = kAnchorBottom - anchorPoint.y;

	// 左右反転
	if (isFlipX_) {
		left = -left;
		right = -right;
	}
	// 上下反転
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}

	// テクスチャ範囲指定の反映
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance().GetMetaData(textureIndex_);
	float tex_left = textureLeftTop_.x / metadata.width;
	float tex_right = (textureLeftTop_.x + textureSize_.x) / metadata.width;
	float tex_top = textureLeftTop_.y / metadata.height;
	float tex_bottom = (textureLeftTop_.y + textureSize_.y) / metadata.height;

	// 頂点リソースにデータを書き込む
	// 左下
	vertexData_[kVertexIndexLeftBottom].position = { left, bottom, kDefaultTranslation.z, kVertexPositionW };
	vertexData_[kVertexIndexLeftBottom].texcoord = { tex_left, tex_bottom };
	vertexData_[kVertexIndexLeftBottom].normal = kSpriteNormal;
	// 左上
	vertexData_[kVertexIndexLeftTop].position = { left, top, kDefaultTranslation.z, kVertexPositionW };
	vertexData_[kVertexIndexLeftTop].texcoord = { tex_left, tex_top };
	vertexData_[kVertexIndexLeftTop].normal = kSpriteNormal;
	// 右下
	vertexData_[kVertexIndexRightBottom].position = { right, bottom, kDefaultTranslation.z, kVertexPositionW };
	vertexData_[kVertexIndexRightBottom].texcoord = { tex_right, tex_bottom };
	vertexData_[kVertexIndexRightBottom].normal = kSpriteNormal;
	// 右上
	vertexData_[kVertexIndexRightTop].position = { right, top, kDefaultTranslation.z, kVertexPositionW };
	vertexData_[kVertexIndexRightTop].texcoord = { tex_right, tex_top };
	vertexData_[kVertexIndexRightTop].normal = kSpriteNormal;

	// インデックスリソースにデータを書き込む
	indexData_[kIndex0] = kVertexIndexLeftBottom;
	indexData_[kIndex1] = kVertexIndexLeftTop;
	indexData_[kIndex2] = kVertexIndexRightBottom;
	indexData_[kIndex3] = kVertexIndexLeftTop;
	indexData_[kIndex4] = kVertexIndexRightTop;
	indexData_[kIndex5] = kVertexIndexRightBottom;

	// Transform情報を作る
	Matrix worldMatrix = transform_.MakeAffineMatrix();
	Matrix viewMatrix = Matrix::Identity();
	Matrix projectionMatrix = Matrix::Orthographic(static_cast<float>(Window::GetWidth()), static_cast<float>(Window::GetHeight()), kNearClip, kFarClip);
	Matrix worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
	transformationMatrixData_->WVP = worldViewProjectionMatrix;
	transformationMatrixData_->World = worldMatrix;
}

void Sprite::Draw() {
	// VertexBufferViewを設定
	spriteCommon_->GetDxBase()->GetCommandList()->IASetVertexBuffers(kVertexBufferSlot, kVertexBufferCount, &vertexBufferView_);
	// IBVを設定
	spriteCommon_->GetDxBase()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	// マテリアルCBufferの場所を設定
	spriteCommon_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(kMaterialRootParameterIndex, materialResource_->GetGPUVirtualAddress());
	// TransformatinMatrixCBufferの場所を設定
	spriteCommon_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(kTransformRootParameterIndex, transformationMatrixResource_->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定
	TextureManager::SetDescriptorTable(kTextureRootParameterIndex, spriteCommon_->GetDxBase()->GetCommandList(), textureIndex_);
	// 描画（DrawCall/ドローコール）6個のインデックスを使用し1つのインスタンスを描画
	spriteCommon_->GetDxBase()->GetCommandList()->DrawIndexedInstanced(kDrawIndexCount, kDrawInstanceCount, kDrawStartIndexLocation, kDrawBaseVertexLocation, kDrawStartInstanceLocation);
}

void Sprite::AdjustTextureSize() {
	// テクスチャメタデータを取得
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance().GetMetaData(textureIndex_);

	textureSize_.x = static_cast<float>(metadata.width);
	textureSize_.y = static_cast<float>(metadata.height);
	// 画像サイズをテクスチャサイズに合わせる
	size_ = textureSize_;
}
