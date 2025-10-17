#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <d3d12.h>
#include <stdint.h>
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")

// =========================================================
// ディスクリプタヒープのラッパークラス
// =========================================================
class DescriptorHeap {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// ディスクリプタヒープを作成します。
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="heapType">ヒープタイプ</param>
	/// <param name="numDescriptors">ディスクリプタ数</param>
	/// <param name="shaderVisible">シェーダ可視性</param>
	void Create(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// CPUディスクリプタハンドルを取得します。
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>CPUディスクリプタハンドル</returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t index);

	/// <summary>
	/// GPUディスクリプタハンドルを取得します。
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>GPUディスクリプタハンドル</returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t index);

	// =========================================================
	// Getter / Setter
	// =========================================================

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_;			/* ディスクリプタヒープ */

private:
	D3D12_DESCRIPTOR_HEAP_TYPE type_;							/* ヒープタイプ */
	uint32_t size_;												/* ディスクリプタサイズ */
};
