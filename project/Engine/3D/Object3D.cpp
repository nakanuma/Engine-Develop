#include "Object3D.h"

// C++
#include <numbers>

// Engine
#include <Camera.h>
#include <SRVManager.h>
#include <LightCamera.h>
#include <CommandManager.h>

Cygnus::Object3D::Object3D() {
	transform_.translate_ = kDefaultTranslate;
	transform_.rotate_ = kDefaultRotation;
	transform_.scale_ = kDefaultScale;

	// 白を書き込む
	materialCB_.data_->color = kDefaultColor;
	// ライティング有効化
	materialCB_.data_->enableLighting = true;
	// 環境マップ有効化フラグ（CubeMapをバインドしてない場合には有効化できない）
	materialCB_.data_->useEnvironmentMap = false;
	// 単位行列で初期化
	materialCB_.data_->uvTransform = Matrix::Identity();
	// 光沢を初期化
	materialCB_.data_->shininess = kDefaultShinniness;
	// 環境反射の強度を初期化
	materialCB_.data_->environmentStrength = kDefaultEnvironmentStrength;
	// 発光色を初期化
	materialCB_.data_->emissiveColor = kDefaultEmissiveColor;
	// 発光強度を初期化
	materialCB_.data_->emissiveIntensity = kDefaultEmissiveIntensity;
}

void Cygnus::Object3D::UpdateMatrix() {
	Matrix worldMatrix = CalculateWorldMatrix();

	Matrix viewMatrix = Camera::GetCurrent()->MakeViewMatrix();
	Matrix projectionMatrix = Camera::GetCurrent()->MakePerspectiveFovMatrix();
	Matrix worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
	wvpCB_.data_->WVP = worldViewProjectionMatrix;
	wvpCB_.data_->World = worldMatrix;

	// 逆転置行列を求める
	Matrix worldInverseMatrix = Matrix::Inverse(worldMatrix);
	Matrix worldInverseTransposeMatrix = Matrix::Transpose(worldInverseMatrix);

	wvpCB_.data_->WorldInverseTranspose = worldInverseTransposeMatrix;
}

void Cygnus::Object3D::UpdateShadowMatrix() {
	Matrix worldMatrix = CalculateWorldMatrix();

	shadowWvpCB_.data_->World = worldMatrix;
	shadowWvpCB_.data_->LightViewProj = LightCamera::GetInstance()->GetViewProj();
}

void Cygnus::Object3D::ScaleUV(float scaleU) {
	// UV変換行列を作成する（U方向にスケール）
	Matrix uvScaleMatrix = Matrix::Scaling({scaleU, kDefaultUVScale, kDefaultUVScale});

	// マテリアルのUV変換行列にスケールを適用
	materialCB_.data_->uvTransform = uvScaleMatrix;
}

void Cygnus::Object3D::Draw() {
	auto cmd = CommandManager::GetInstance()->GetCommandList();

	// 共通セットアップ
	DrawSetup();

	// commandListにVBVを設定
	cmd->IASetVertexBuffers(kMeshVBVStartSlot, kMeshVBVCount, &model_->vertexBufferView);
	// 描画を行う（DrawCall/ドローコール）
	cmd->DrawIndexedInstanced(static_cast<UINT>(model_->indices.size()), 1, 0, 0, 0);
}

void Cygnus::Object3D::Draw(const SkinCluster& skinCluster) {
	auto cmd = CommandManager::GetInstance()->GetCommandList();

	// 共通セットアップ
	DrawSetup();

	D3D12_VERTEX_BUFFER_VIEW vbvs[kSkinMeshVBVCount] = {
	    model_->vertexBufferView,        // VertexDataのVBV
	    skinCluster.influenceBufferView_ // InfluenceのVBV
	};

	// 配列を渡す（開始Slot番号、使用Slot番号、VBV配列へのポインタ）
	cmd->IASetVertexBuffers(kMeshVBVStartSlot, kSkinMeshVBVCount, vbvs);
	// PaletteのSRVを設定
	cmd->SetGraphicsRootDescriptorTable(kRootParameterIndexSkinPaletteSRV, skinCluster.paletteSrvHandle_.second);
	// 描画を行う（DrawCall/ドローコール）
	cmd->DrawIndexedInstanced(static_cast<UINT>(model_->indices.size()), 1, 0, 0, 0);
}

void Cygnus::Object3D::DrawShadow() {
	auto cmd = CommandManager::GetInstance()->GetCommandList();

	// 共通セットアップ
	DrawShadowSetup();

	// 頂点バッファの設定
	cmd->IASetVertexBuffers(kMeshVBVStartSlot, kMeshVBVCount, &model_->vertexBufferView);

	// DrawCall
	cmd->DrawIndexedInstanced(static_cast<UINT>(model_->indices.size()), 1, 0, 0, 0);
}

void Cygnus::Object3D::DrawShadow(const SkinCluster& skinCluster) {
	auto cmd = CommandManager::GetInstance()->GetCommandList();

	// 共通セットアップ
	DrawShadowSetup();

	// 頂点バッファの設定
	D3D12_VERTEX_BUFFER_VIEW vbvs[kSkinMeshVBVCount] = {model_->vertexBufferView, skinCluster.influenceBufferView_};
	cmd->IASetVertexBuffers(kMeshVBVStartSlot, kSkinMeshVBVCount, vbvs);

	// DrawCall
	cmd->DrawIndexedInstanced(static_cast<UINT>(model_->indices.size()), 1, 0, 0, 0);
}

void Cygnus::Object3D::SetEmissive(const Float3& color, float intensity, float radius, float decay) {
	isEmissive_ = true;
	materialCB_.data_->emissiveColor = color;
	materialCB_.data_->emissiveIntensity = intensity;
	emissiveRadius_ = radius;
	emissiveDecay_ = decay;
}

void Cygnus::Object3D::UpdateEmissiveLight() {
	if (!isEmissive_)
		return;

	// ワールド座標を計算
	Matrix worldMatrix = transform_.MakeAffineMatrix();
	if (parent_) {
		Matrix parentWorldMatrix = parent_->transform_.MakeAffineMatrix();
		worldMatrix = worldMatrix * parentWorldMatrix;
	}

	// ワールド座標の位置を取得
	Float3 worldPosition = {worldMatrix.r[3][0], worldMatrix.r[3][1], worldMatrix.r[3][2]};

	// LightManagerに登録
	LightManager::GetInstance()->RegisterEmissiveLight(worldPosition, materialCB_.data_->emissiveColor, materialCB_.data_->emissiveIntensity, emissiveRadius_, emissiveDecay_);
}

void Cygnus::Object3D::SetEmissiveAsAreaLight(const Float3& color, float intensity, float range, LightManager::AreaLightType type) {
	isEmissive_ = true;
	materialCB_.data_->emissiveColor = color;
	materialCB_.data_->emissiveIntensity = intensity;
	emissiveRadius_ = range;
	emissiveAreaLightType_ = type;
}

void Cygnus::Object3D::UpdateEmissiveAreaLight() {
	if (!isEmissive_)
		return;

	// ワールド行列を計算
	Matrix worldMatrix = transform_.MakeAffineMatrix();
	if (parent_) {
		Matrix parentWorldMatrix = parent_->transform_.MakeAffineMatrix();
		worldMatrix = worldMatrix * parentWorldMatrix;
	}

	// ワールド座標系での位置を取得
	Float3 position = {worldMatrix.r[3][0], worldMatrix.r[3][1], worldMatrix.r[3][2]};

	// Tubeタイプの場合
	if (emissiveAreaLightType_ == LightManager::AreaLightType::Tube) {
		// ワールド座標系での方向を取得
		Float3 right = {worldMatrix.r[0][0], worldMatrix.r[0][1], worldMatrix.r[0][2]};

		// チューブの長さを計算
		float length = transform_.scale_.x;

		// チューブの両端点を計算
		Float3 halfRight = right * (length * 0.5f);
		Float3 start = position - halfRight;
		Float3 end = position + halfRight;

		// RightManagerに登録
		LightManager::GetInstance()->RegisterTubeLight(
			start,
			end, 
			materialCB_.data_->emissiveColor, 
			materialCB_.data_->emissiveIntensity, 
			emissiveRadius_
		);
	// Rectangle, Disk, Sphereの場合
	} else {
		// ワールド座標系での方向を取得
		Float3 right = {worldMatrix.r[0][0], worldMatrix.r[0][1], worldMatrix.r[0][2]};
		Float3 up = {worldMatrix.r[1][0], worldMatrix.r[1][1], worldMatrix.r[1][2]};

		// スケールを考慮した幅と高さを計算
		float width = transform_.scale_.x * 2.0f;
		float height = transform_.scale_.y * 2.0f;

		// エリアライトとしてRightManagerに登録
		LightManager::GetInstance()->RegisterAreaLight(
		    position, 
			right, 
			up, 
			width, 
			height, 
			materialCB_.data_->emissiveColor, 
			materialCB_.data_->emissiveIntensity, 
			emissiveRadius_, emissiveAreaLightType_
		);
	}
}

Cygnus::Matrix Cygnus::Object3D::CalculateWorldMatrix()
{
	Matrix worldMatrix = transform_.MakeAffineMatrix();
	// 親が存在する場合、親の行列を考慮する
	if (parent_) {
		Matrix parentWorldMatrix = parent_->transform_.MakeAffineMatrix(); // 親のワールド行列
		worldMatrix = worldMatrix * parentWorldMatrix;                     // 子の行列に親の行列を掛ける
	}
	return worldMatrix;
}

void Cygnus::Object3D::DrawSetup()
{
	auto cmd = CommandManager::GetInstance()->GetCommandList();

	// commandListにIBVを設定
	cmd->IASetIndexBuffer(&model_->indexBufferView);
	// プリミティブトポロジーの設定
	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexMaterial, materialCB_.resource_->GetGPUVirtualAddress());
	// wvp用のCBufferの場所を設定
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexWVP, wvpCB_.resource_->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定（Textureの設定）
	TextureManager::SetDescriptorTable(kRootParameterIndexTexture, cmd, model_->material.textureHandle); // モデルデータに格納されたテクスチャを使用する
}

void Cygnus::Object3D::DrawShadowSetup()
{
	auto cmd = CommandManager::GetInstance()->GetCommandList();

	// インデックスバッファの設定
	cmd->IASetIndexBuffer(&model_->indexBufferView);
	// プリミティブトポロジーの設定
	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// WVP（World * LightViewProj）のみを使う
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexShadowCBV, shadowWvpCB_.resource_->GetGPUVirtualAddress());
}
