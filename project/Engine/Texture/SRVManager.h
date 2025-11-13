#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <DescriptorHeap.h>
#include <DirectXBase.h>

// =========================================================
// シェーダーリソースビュー（SRV）管理クラス
// =========================================================
class SRVManager {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static SRVManager* GetInstance();

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	/// <param name="dxBase">DirectX基盤クラス</param>
	void Initialize(DirectXBase* dxBase);
	
	/// <summary>
	/// テクスチャ用のSRVを生成します。
	/// </summary>
	/// <param name="srvIndex">SRVのインデックス</param>
	/// <param name="pResource">リソース</param>
	/// <param name="Format">フォーマット</param>
	/// <param name="MipLevels">ミップレベル</param>
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);

	/// <summary>
	/// Structured Buffer用のSRVを生成します。
	/// </summary>
	/// <param name="srvIndex">SRVのインデックス</param>
	/// <param name="pResource">リソース</param>
	/// <param name="numElements">要素数</param>
	/// <param name="structureByteStride">構造体のバイトストライド</param>
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

	/// <summary>
	/// SRVを割り当てます。
	/// </summary>
	/// <returns>SRVのインデックス</returns>
	uint32_t Allocate();

	/// <summary>
	/// 描画前処理を行います。
	/// </summary>
	void PreDraw();

	/// <summary>
	/// グラフィックスルートディスクリプタテーブルを設定します。
	/// </summary>
	/// <param name="RootParameterIndex">ルートパラメータインデックス</param>
	/// <param name="srvIndex">SRVインデックス</param>
	void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);

	/// <summary>
	/// CPU デスクリプタハンドルを取得します。
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>CPU デスクリプタハンドル</returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// GPU デスクリプタハンドルを取得します。
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>GPU デスクリプタハンドル</returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// SRVのインデックスを取得します。
	/// </summary>
	/// <returns>SRVのインデックス</returns>
	const uint32_t GetIndex() { return useIndex_; }

	/// <summary>
	/// SRVのインデックスをインクリメントします。
	/// </summary>
	void IncrementIndex() { useIndex_++; }

	/// <summary>
	/// SRVを割り当てることができるか確認します。
	/// </summary>
	/// <returns>true: 割り当て可能, false: 割り当て不可能</returns>
	bool CanAllocate();

	// =========================================================
	// Member Variables
	// =========================================================

	DescriptorHeap descriptorHeap_;			/* ディスクリプタヒープ */

private:
	DirectXBase* dxBase_ = nullptr;			/* DirectX基盤クラス */

	static const uint32_t kMaxSRVCount;		/* 最大SRV数 */
	uint32_t descriptorSize_;				/* ディスクリプタサイズ */
	uint32_t useIndex_ = 1;					/* 使用中のSRVインデックス */
};
