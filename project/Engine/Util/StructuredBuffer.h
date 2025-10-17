#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include "DescriptorHeap.h"
#include "DirectXBase.h"
#include "DirectXUtil.h"
#include "SRVManager.h"
#include "TextureManager.h"

// =========================================================
// ストラクチャードバッファのラッパークラス
// =========================================================
template<class Type> class StructuredBuffer {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="numInstance">インスタンス数</param>
	/// <param name="isEmpty">trueなら空のバッファを作成する</param>
	StructuredBuffer(uint32_t numInstance, bool isEmpty = false) : numMaxInstance_(numInstance) {
		if (!isEmpty)
			Create();
	};

	/// <summary>
	/// ストラクチャードバッファを作成します。
	/// </summary>
	void Create() {
		// リソースを作る
		resource_ = CreateBufferResource(DirectXBase::GetInstance()->GetDevice(), sizeof(Type) * numMaxInstance_);
		// データを書き込む
		data_ = nullptr;
		// 書き込むためのアドレスを取得
		resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
		// DescriptorHeapの生成
		/*heap_.Create(DirectXBase::GetInstance()->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kNumInstance, true);*/

		CreateSRV();
	};

	/// <summary>
	/// ストラクチャードバッファのSRVを作成します。
	/// </summary>
	void CreateSRV();

	/// <summary>
	/// コピー不可
	/// </summary>
	StructuredBuffer(const StructuredBuffer&) = delete;
	StructuredBuffer(StructuredBuffer&&) = delete;
	StructuredBuffer& operator=(const StructuredBuffer&) = delete;
	StructuredBuffer& operator=(StructuredBuffer&&) = delete;

	// =========================================================
	// Member Variables
	// =========================================================

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;	/* ストラクチャードバッファリソース */
	Type* data_;										/* ストラクチャードバッファデータ ポインタ */

	uint32_t numMaxInstance_;							/* インスタンス最大数 */
	uint32_t heapIndex_;								/* ヒープのインデックス */
};

/// <summary>
/// ストラクチャードバッファのSRVを作成します。
/// </summary>
template<class Type> inline void StructuredBuffer<Type>::CreateSRV() {
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = numMaxInstance_;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(Type);
	/*D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU = SRVManager::GetInstance().descriptorHeap.GetCPUHandle(SRVManager::GetInstance().GetIndex());*/
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU = SRVManager::GetInstance()->descriptorHeap.GetCPUHandle(SRVManager::GetInstance()->GetIndex());
	heapIndex_ = SRVManager::GetInstance()->GetIndex(); // heapのIndexを記録
	SRVManager::GetInstance()->IncrementIndex();        // indexをインクリメント
	DirectXBase::GetInstance()->GetDevice()->CreateShaderResourceView(resource_.Get(), &instancingSrvDesc, instancingSrvHandleCPU);
}