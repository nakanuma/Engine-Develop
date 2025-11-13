#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <memory>
#include <stdint.h>
#include <unordered_map>
#include <wrl.h>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Float4.h>
#include <DirectXBase.h>

// =========================================================
//  レンダーターゲットビュー（RTV）管理クラス
// =========================================================
class RTVManager final {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static RTVManager& GetInstance();

	/// <summary>
	/// レンダーターゲットテクスチャを作成します。
	/// </summary>
	/// <param name="width">幅</param>
	/// <param name="height">高さ</param>
	/// <param name="clearColor">クリアカラー</param>
	/// <returns>テクスチャハンドル</returns>
	static int32_t CreateRenderTargetTexture(uint32_t width, uint32_t height, Float4 clearColor = {0.1f, 0.25f, 0.5f, 1.0f});

	/// <summary>
	/// レンダーターゲットを設定します。
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	static void SetRenderTarget(int32_t textureHandle);

	/// <summary>
	/// レンダーターゲットをバックバッファに設定します。
	/// </summary>
	static void SetRTtoBB();

	/// <summary>
	/// リソースバリアをリセットします。
	/// </summary>
	static void ResetResourceBarrier();

	/// <summary>
	/// レンダーターゲットをクリアします。
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <param name="clearColor">クリアカラー</param>
	static void ClearRTV(int32_t textureHandle, Float4 clearColor = {0.1f, 0.25f, 0.5f, 1.0f});

	/// <summary>
	/// デプスステンシルビュー（DSV）ハンドルを取得します。
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <returns>デプスステンシルビュー（DSV）ハンドル</returns>
	static int32_t GetDepthSRVHandle(int32_t textureHandle);

private:
	// =========================================================
	// Member Variables
	// =========================================================

	std::unordered_map<int32_t, int32_t> rtvHandleMap_;										/* レンダーターゲットビュー（RTV）ハンドルマップ */
	int32_t rtvIndex_ = 2;																	/* レンダーターゲットビュー（RTV）ハンドルインデックス */

	int32_t currentRenderTarget_ = -1;														/* 現在のレンダーターゲット状態 */

	std::unordered_map<int32_t, Microsoft::WRL::ComPtr<ID3D12Resource>> dsvResourceMap_;	/* デプスステンシルビュー（DSV）リソースマップ */
	std::unordered_map<int32_t, int32_t> depthSRVHandleMap_;								/* デプスシェンシルビュー（DSV）シェーダリソースビュー（SRV）ハンドルマップ */
};
