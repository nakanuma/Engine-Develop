#include "InstancedObject.h"

// Engine
#include <CommandManager.h>

Cygnus::InstancedObject::InstancedObject() { Object3D::Object3D(); }

void Cygnus::InstancedObject::InstancedDraw() {
	auto cmd = CommandManager::GetInstance()->GetCommandList();

	// commandListにVBVを設定
	cmd->IASetVertexBuffers(0, 1, &model_->vertexBufferView);
	// commandListにIBVを設定
	cmd->IASetIndexBuffer(&model_->indexBufferView);
	// プリミティブトポロジーの設定
	cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexMaterial, materialCB_.resource_->GetGPUVirtualAddress());
	// wvp用のCBufferの場所を設定
	cmd->SetGraphicsRootConstantBufferView(kRootParameterIndexWVP, wvpCB_.resource_->GetGPUVirtualAddress());
	// instancing用のDataを読むためにStructuredBufferのSRVを設定
	cmd->SetGraphicsRootDescriptorTable(kRootParameterIndexStructuredBuffer, SRVManager::GetInstance()->descriptorHeap_.GetGPUHandle(gTransformationMatrices_.heapIndex_));
	// SRVのDescriptorTableの先頭を設定（Textureの設定）
	TextureManager::SetDescriptorTable(kRootParameterIndexTexture, cmd, model_->material.textureHandle); // モデルデータに格納されたテクスチャを使用する
	// 描画を行う（DrawCall/ドローコール）
	cmd->DrawIndexedInstanced(static_cast<UINT>(model_->indices.size()), gTransformationMatrices_.numMaxInstance_, 0, 0, 0);
}