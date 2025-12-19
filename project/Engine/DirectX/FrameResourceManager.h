#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <cstdint>
#include <array>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <DescriptorHeap.h>

namespace Cygnus{
// =========================================================
// フレームリソース管理クラス
// =========================================================
class FrameResourceManager
{
public:
	/// <summary>
	/// シングルトンインスタンスを取得します。
	/// </summary>
	/// <returns></returns>
	static FrameResourceManager* GetInstance();

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="swapChain">スワップチェーン</param>
	void Initialize(ID3D12Device* device, IDXGISwapChain4* swapChain);

	/// <summary>
	/// フレーム開始時の処理を行います。
	/// </summary>
	/// <param name="commandList"></param>
	void BeginFrame(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// レンダーターゲットをクリアします。
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="clearColor">クリアカラー</param>
	void ClearRenderTarget(ID3D12GraphicsCommandList* commandList, const float clearColor[4]);

	/// <summary>
	/// 深度バッファをクリアします。
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="depth">クリアする深度値</param>
	void ClearDepthStencil(ID3D12GraphicsCommandList* commandList, float depth = 1.0f);

	/// <summary>
	/// レンダーターゲットと深度ステンシルビューを設定します。
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	void SetRenderTargets(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// バックバッファをPresent可能な状態に遷移します。
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	void TransitionToPresent(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// 現在のバックバッファインデックスを取得します。
	/// </summary>
	/// <returns>インデックス</returns>
	UINT GetCurrentBackBufferIndex() const;
 
	/// <summary>
	/// 現在のレンダーターゲットビューハンドルを取得します。
	/// </summary>
	/// <returns>RTVハンドル</returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle() const;

	/// <summary>
	/// 現在のバックバッファリソースを取得します。
	/// </summary>
	/// <returns>バックバッファリソース</returns>
	ID3D12Resource* GetCurrentBackBufferResource() const;

	/// <summary>
	/// 指定したインデックスのレンダーターゲットビューハンドルを取得します。
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>RTVハンドル</returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(UINT index) const;

	/// <summary>
	/// 深度ステンシルビューハンドルを取得します。
	/// </summary>
	/// <returns>DSVハンドル</returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const;

	/// <summary>
	/// 深度ステンシルリソースを取得します。
	/// </summary>
	/// <returns>深度ステンシルリソース</returns>
	ID3D12Resource* GetDepthStencilResource() const { return depthStencilResource_.Get(); }

	/// <summary>
	/// RTVディスクリプタヒープを取得します。
	/// </summary>
	/// <returns></returns>
	Cygnus::DescriptorHeap* GetRTVHeap() { return &rtvDescriptorHeap_; }

	/// <summary>
	/// DSVディスクリプタヒープを取得します。
	/// </summary>
	/// <returns>ディスクリプタヒープ</returns>
	Cygnus::DescriptorHeap* GetDSVHeap() { return &dsvDescriptorHeap_; }

	/// <summary>
	/// RTV設定を取得します。
	/// </summary>
	/// <returns>RTV設定</returns>
	D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc() const { return rtvDesc_; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// レンダーターゲットを生成します。
	/// </summary>
	void CreateRenderTargets();

	/// <summary>
	/// 深度ステンシルバッファを生成します。
	/// </summary>
	/// <param name="width">バッファの幅</param>
	/// <param name="height">バッファの高さ</param>
	void CreateDepthStencilBuffer(uint32_t width, uint32_t height);

	/// <summary>
	/// リソースバリアを設定します。
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="resource">対象リソース</param>
	/// <param name="stateBefore">遷移前の状態</param>
	/// <param name="stateAfter">遷移後の状態</param>
	void TransitionResource(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

private:
	// =========================================================
	// Constants
	// =========================================================

	static constexpr size_t kBackBufferCount = 2;	/* バックバッファの数 */
	static constexpr uint32_t kRTVHeapSize = 128;	/* RTVヒープのサイズ */
	static constexpr uint32_t kDSVHeapSize = 128;	/* DSVヒープのサイズ */

	static constexpr float kDefaultClearColor[4] = {0.1f, 0.25f, 0.5f, 1.0f}; /* デフォルトのクリアカラー */

	// =========================================================
	// Member Variables
	// =========================================================

	ID3D12Device* device_ = nullptr;
	IDXGISwapChain4* swapChain_ = nullptr;

	// レンダーターゲット関連
	Cygnus::DescriptorHeap rtvDescriptorHeap_;
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kBackBufferCount> swapChainResource_;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kBackBufferCount> rtvHandles_;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};

	// 深度ステンシル関連
	Cygnus::DescriptorHeap dsvDescriptorHeap_;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_{};

	// バリア用
	D3D12_RESOURCE_BARRIER barrier_{};

	// 現在のバックバッファインデックス
	UINT currentBackBufferIndex_ = 0;
};
}

