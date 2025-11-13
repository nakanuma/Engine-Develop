#include "InstancedObject.h"

InstancedObject::InstancedObject() { Object3D::Object3D(); }

void InstancedObject::InstancedDraw() {
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
	// instancing用のDataを読むためにStructuredBufferのSRVを設定
	dxBase->GetCommandList()->SetGraphicsRootDescriptorTable(5, SRVManager::GetInstance()->descriptorHeap_.GetGPUHandle(gTransformationMatrices_.heapIndex_));
	// SRVのDescriptorTableの先頭を設定（Textureの設定）
	TextureManager::SetDescriptorTable(2, dxBase->GetCommandList(), model_->material.textureHandle); // モデルデータに格納されたテクスチャを使用する
	// 描画を行う（DrawCall/ドローコール）
	dxBase->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(model_->indices.size()), gTransformationMatrices_.numMaxInstance_, 0, 0, 0);
}