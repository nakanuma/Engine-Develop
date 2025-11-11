#include "Object3D.h"
#include "Camera.h"
#include "SRVManager.h"
#include <LightCamera.h>

#include <numbers>

Object3D::Object3D() {
	transform_.translate = {0.0f, 0.0f, 0.0f};
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.scale = {1.0f, 1.0f, 1.0f};

	// 白を書き込む
	materialCB_.data_->color = {1.0f, 1.0f, 1.0f, 1.0f};
	// ライティング有効化
	materialCB_.data_->enableLighting = true;
	// 環境マップ有効化フラグ（CubeMapをバインドしてない場合には有効化できない）
	materialCB_.data_->useEnvironmentMap = false;
	// 単位行列で初期化
	materialCB_.data_->uvTransform = Matrix::Identity();
	// 光沢を初期化
	materialCB_.data_->shininess = 50.0f;
	// 環境反射の強度を初期化
	materialCB_.data_->environmentStrength = 1.0f;
}

void Object3D::UpdateMatrix() {
	Matrix worldMatrix = transform_.MakeAffineMatrix();
	// 親が存在する場合、親の行列を考慮する
	if (parent_) {
		Matrix parentWorldMatrix = parent_->transform_.MakeAffineMatrix(); // 親のワールド行列
		worldMatrix = worldMatrix * parentWorldMatrix;                     // 子の行列に親の行列を掛ける
	}

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

void Object3D::UpdateShadowMatrix() {
	Matrix worldMatrix = transform_.MakeAffineMatrix();

	// 親が存在する場合、親の行列を考慮する
	if (parent_) {
		Matrix parentWorldMatrix = parent_->transform_.MakeAffineMatrix(); // 親のワールド行列
		worldMatrix = worldMatrix * parentWorldMatrix;                     // 子の行列に親の行列を掛ける
	}

	shadowWvpCB_.data_->World = worldMatrix;
	shadowWvpCB_.data_->LightViewProj = LightCamera::GetInstance()->GetViewProj();
}

void Object3D::ScaleUV(float scaleU) {
	// UV変換行列を作成する（U方向にスケール）
	Matrix uvScaleMatrix = Matrix::Scaling({scaleU, 1.0f, 1.0f});

	// マテリアルのUV変換行列にスケールを適用
	materialCB_.data_->uvTransform = uvScaleMatrix;
}

void Object3D::Draw() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// commandListにVBVを設定
	dxBase->GetCommandList()->IASetVertexBuffers(0, 1, &model_->vertexBufferView);
	// commandListにIBVを設定
	dxBase->GetCommandList()->IASetIndexBuffer(&model_->indexBufferView);
	// プリミティブトポロジーの設定
	dxBase->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialCB_.resource_->GetGPUVirtualAddress());
	// wvp用のCBufferの場所を設定
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpCB_.resource_->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定（Textureの設定）
	TextureManager::SetDescriptorTable(2, dxBase->GetCommandList(), model_->material.textureHandle); // モデルデータに格納されたテクスチャを使用する
	// 描画を行う（DrawCall/ドローコール）
	dxBase->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(model_->indices.size()), 1, 0, 0, 0);
}

void Object3D::Draw(SkinCluster skinCluster) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
	    model_->vertexBufferView,        // VertexDataのVBV
	    skinCluster.influenceBufferView_ // InfluenceのVBV
	};

	// 配列を渡す（開始Slot番号、使用Slot番号、VBV配列へのポインタ）
	dxBase->GetCommandList()->IASetVertexBuffers(0, 2, vbvs);
	// commandListにIBVを設定
	dxBase->GetCommandList()->IASetIndexBuffer(&model_->indexBufferView);
	// プリミティブトポロジーの設定
	dxBase->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialCB_.resource_->GetGPUVirtualAddress());
	// wvp用のCBufferの場所を設定
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpCB_.resource_->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定（Textureの設定）
	TextureManager::SetDescriptorTable(2, dxBase->GetCommandList(), model_->material.textureHandle); // モデルデータに格納されたテクスチャを使用する
	// PaletteのSRVを設定
	dxBase->GetCommandList()->SetGraphicsRootDescriptorTable(5, skinCluster.paletteSrvHandle_.second);
	// 描画を行う（DrawCall/ドローコール）
	dxBase->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(model_->indices.size()), 1, 0, 0, 0);
}

void Object3D::DrawInstancing(StructuredBuffer<ParticleForGPU>& structuredBuffer, uint32_t numInstance, const uint32_t TextureHandle) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// パーティクル用ルートシグネチャを設定
	dxBase->GetCommandList()->SetGraphicsRootSignature(dxBase->GetRootSignatureParticle());
	// パーティクル用PSOを設定
	dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineStateParticle());
	// commandListにVBVを設定
	dxBase->GetCommandList()->IASetVertexBuffers(0, 1, &model_->vertexBufferView);
	// マテリアルCBufferの場所を設定
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialCB_.resource_->GetGPUVirtualAddress());
	// instancing用のDataを読むためにStructuredBufferのSRVを設定する
	dxBase->GetCommandList()->SetGraphicsRootDescriptorTable(1, SRVManager::GetInstance()->descriptorHeap.GetGPUHandle(structuredBuffer.heapIndex_));
	// SRVのDescriptorTableの先頭を設定（Textureの設定）
	TextureManager::SetDescriptorTable(2, dxBase->GetCommandList(), TextureHandle); // 引数で指定したテクスチャを使用する
	// 描画を行う（DrawCall/ドローコール）
	dxBase->GetCommandList()->DrawInstanced(UINT(model_->vertices.size()), numInstance, 0, 0);
}

void Object3D::DrawPartial(uint32_t indexCount) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// commandListにVBVを設定
	dxBase->GetCommandList()->IASetVertexBuffers(0, 1, &model_->vertexBufferView);
	// commandListにIBVを設定
	dxBase->GetCommandList()->IASetIndexBuffer(&model_->indexBufferView);
	// プリミティブトポロジーの設定
	dxBase->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialCB_.resource_->GetGPUVirtualAddress());
	// wvp用のCBufferの場所を設定
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpCB_.resource_->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定（Textureの設定）
	TextureManager::SetDescriptorTable(2, dxBase->GetCommandList(), model_->material.textureHandle); // モデルデータに格納されたテクスチャを使用する
	// 描画を行う（DrawCall/ドローコール）
	dxBase->GetCommandList()->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
}

void Object3D::DrawShadow() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 頂点バッファ・インデックスバッファの設定
	dxBase->GetCommandList()->IASetVertexBuffers(0, 1, &model_->vertexBufferView);
	dxBase->GetCommandList()->IASetIndexBuffer(&model_->indexBufferView);
	dxBase->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// WVP（World * LightViewProj）のみを使う
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(11, shadowWvpCB_.resource_->GetGPUVirtualAddress());

	// DrawCall
	dxBase->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(model_->indices.size()), 1, 0, 0, 0);
}

void Object3D::DrawShadow(SkinCluster skinCluster) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {model_->vertexBufferView, skinCluster.influenceBufferView_};

	dxBase->GetCommandList()->IASetVertexBuffers(0, 2, vbvs);
	dxBase->GetCommandList()->IASetIndexBuffer(&model_->indexBufferView);
	dxBase->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// World行列とLightViewProj行列の定数バッファを設定
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(11, shadowWvpCB_.resource_->GetGPUVirtualAddress());

	// DrawCall
	dxBase->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(model_->indices.size()), 1, 0, 0, 0);
}
