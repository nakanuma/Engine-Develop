#include "DirectXUtil.h"
#include <assert.h>

Microsoft::WRL::ComPtr<ID3D12Resource> Cygnus::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourcesDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourcesDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourcesDesc.Width = sizeInBytes; // リソースのサイズ
	// バッファの場合はこれらは1にする決まり
	vertexResourcesDesc.Height = 1;
	vertexResourcesDesc.DepthOrArraySize = 1;
	vertexResourcesDesc.MipLevels = 1;
	vertexResourcesDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	vertexResourcesDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際に頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourcesDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	return std::move(vertexResource); // デストラクタを呼ばないように返す
}

Microsoft::WRL::ComPtr<ID3D12Resource> Cygnus::CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height, bool isReading) {
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;                                   // Textureの幅
	resourceDesc.Height = height;                                 // Textureの高さ
	resourceDesc.MipLevels = 1;                                   // mipmapの数
	resourceDesc.DepthOrArraySize = 1;                            // 奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;          // DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;                            // サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;  // 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; /// DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;              // 1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット。Resourceと合わせる

	// Resourceの設定
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
	    &heapProperties,                                                                  // Heapの設定
	    D3D12_HEAP_FLAG_NONE,                                                             // Heapの特殊な設定。特になし
	    &resourceDesc,                                                                    // Resourceの設定
	    isReading ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
	    &depthClearValue,                                                                 // Clear最適値
	    IID_PPV_ARGS(&resource));                                                         // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	return std::move(resource);
}

void Cygnus::TransitionResource(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState) {
	// 既に目的のステートなら早期リターン
	if (beforeState == afterState)
		return;

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; // 遷移タイプ
	barrier.Transition.pResource = resource;               // 遷移対象のリソース
	barrier.Transition.StateBefore = beforeState;          // 現在の状態
	barrier.Transition.StateAfter = afterState;            // 遷移後の状態
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// リソースバリアを発行
	cmdList->ResourceBarrier(1, &barrier);
}