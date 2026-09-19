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

namespace Cygnus {
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
	static int32_t CreateRenderTargetTexture(uint32_t width, uint32_t height, const Float4& clearColor = {0.1f, 0.25f, 0.5f, 1.0f}, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

	/// <summary>
	/// レンダーターゲットを設定します。
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	static void SetRenderTarget(int32_t textureHandle);

	/// <summary>
	/// レンダーターゲットを設定します。（別のレンダーターゲットの深度バッファを使用）
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <param name="depthSourceHandle">深度バッファを使用するテクスチャ</param>
	static void SetRenderTarget(int32_t textureHandle, int32_t depthSourceHandle);

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
	static void ClearRTV(int32_t textureHandle, const Float4& clearColor = {0.1f, 0.25f, 0.5f, 1.0f});

	/// <summary>
	/// Depthのみクリアします。
	/// </summary>
	/// <param name="textureHandle"></param>
	static void ClearDepth(int32_t textureHandle);

	/// <summary>
	/// デプスステンシルビュー（DSV）ハンドルを取得します。
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <returns>デプスステンシルビュー（DSV）ハンドル</returns>
	static int32_t GetDepthSRVHandle(int32_t textureHandle);

	/// <summary>
	/// デプスステンシルビュー（DSV）ハンドルを取得します。
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <returns>デプスステンシルビュー（DSV）ハンドル</returns>
	static D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle(uint32_t textureHandle);

	/// <summary>
	/// 深度リソースを取得します。
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <returns>深度リソース</returns>
	static ID3D12Resource* GetDepthResource(uint32_t textureHandle);

	static void SetDepthOnlyRenderTarget(int32_t textureHandle);

	static void TransitionDepthToShaderResource(int32_t textureHandle);

	/// <summary>
	/// Depthを変更しない
	/// </summary>
	/// <param name="textureHandle"></param>
	static void SetRenderTargetKeepDepth(int32_t textureHandle);

	/// <summary>
	/// CubeMap用のレンダーターゲットを設定します。
	/// </summary>
	/// <param name="textureHandle"></param>
	/// <param name="mipLevel"></param>
	/// <param name="face"></param>
	static void SetCubeMapRenderTarget(int32_t textureHandle, uint32_t mipLevel, uint32_t face);

	/// <summary>
	/// Cubemapの指定したMip・Faceをシェーダーリソース状態へ戻します。
	/// </summary>
	/// <param name="textureHandle"></param>
	/// <param name="mipLevel"></param>
	/// <param name="face"></param>
	static void ResetCubeMapResourceBarrier(int32_t textureHandle, uint32_t mipLevel, uint32_t face);

	/// <summary>
	/// Cubemapの指定したMip・Faceをクリアします。
	/// </summary>
	/// <param name="textureHandle"></param>
	/// <param name="mipLevel"></param>
	/// <param name="face"></param>
	/// <param name="clearColor"></param>
	static void ClearCubeMapRTV(int32_t textureHandle, uint32_t mipLevel, uint32_t face, const Float4& clearColor = {0.0f, 0.0f, 0.0f, 1.0f});

	static void CreateCubeMapDepth(int32_t textureHandle, uint32_t width, uint32_t height);

	static void ClearRenderTargetOnly(int32_t textureHandle, const Float4& clearColor = {0.0f, 0.0f, 0.0f, 1.0f});

	static void ClearCubeMapDepth(int32_t textureHandle);

private:
	// =========================================================
	// Member Variables
	// =========================================================

	std::unordered_map<int32_t, int32_t> rtvHandleMap_;										/* レンダーターゲットビュー（RTV）ハンドルマップ */
	int32_t rtvIndex_ = 2;																	/* レンダーターゲットビュー（RTV）ハンドルインデックス */

	int32_t currentRenderTarget_ = -1;														/* 現在のレンダーターゲット状態 */

	std::unordered_map<int32_t, Microsoft::WRL::ComPtr<ID3D12Resource>> dsvResourceMap_;	/* デプスステンシルビュー（DSV）リソースマップ */
	std::unordered_map<int32_t, int32_t> depthSRVHandleMap_;								/* デプスシェンシルビュー（DSV）シェーダリソースビュー（SRV）ハンドルマップ */

	std::unordered_map<int32_t, D3D12_CPU_DESCRIPTOR_HANDLE> dsvHandleMap_; // DSVインデックス管理用

	std::unordered_map<int64_t, D3D12_CPU_DESCRIPTOR_HANDLE> cubeMapRTVHandleMap_; // Cubemap用RTV管理

	std::unordered_map<int32_t, Microsoft::WRL::ComPtr<ID3D12Resource>> cubeMapDepthResourceMap_;

	std::unordered_map<int32_t, D3D12_CPU_DESCRIPTOR_HANDLE> cubeMapDSVHandleMap_;
};
}
