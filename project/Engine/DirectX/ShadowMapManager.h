#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <d3d12.h>
#include <stdint.h>
#include <unordered_map>
#include <wrl.h>

// =========================================================
// シャドウマップ管理クラス
// =========================================================
class ShadowMapManager final {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static ShadowMapManager* GetInstance();

	/// <summary>
	/// シャドウマップ管理クラスの初期化処理を行います。
	/// </summary>
	void Initialize();

	/// <summary>
	/// シャドウマップを生成します。
	/// </summary>
	/// <param name="width">幅</param>
	/// <param name="height">高さ</param>
	/// <returns>シャドウマップハンドル</returns>
	int32_t CreateShadowMap(uint32_t width, uint32_t height);

	/// <summary>
	/// シャドウマップ書き込みを開始します。
	/// </summary>
	/// <param name="shadowMapHandle">シャドウマップハンドル</param>
	void BeginShadowPass(uint32_t shadowMapHandle);

	/// <summary>
	/// シャドウマップ書き込みを終了します。
	/// </summary>
	/// <param name="shadowMapHandle">シャドウマップハンドル</param>
	void EndShadowPass(uint32_t shadowMapHandle);

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// シャドウマップのSRVハンドルを取得します。（通常描画パスで使用）
	/// </summary>
	/// <param name="handle">シャドウマップハンドル</param>
	/// <returns>シャドウマップのSRVハンドル</returns>
	int32_t GetShadowSRVHandle(int32_t handle);

	/// <summary>
	/// シャドウマップ描画用PSOを取得します。
	/// </summary>
	/// <returns>シャドウマップ描画用PSO</returns>
	ID3D12PipelineState* GetShadowPSO() { return shadowPSO_.Get(); }

	/// <summary>
	/// スキニング用シャドウマップ描画用PSOを取得します。
	/// </summary>
	/// <returns>スキニング用シャドウマップ描画用PSO</returns>
	ID3D12PipelineState* GetShadowSkinnedPSO() { return shadowSkinnedPSO_.Get(); }

	/// <summary>
	/// シャドウマップのリソースを取得します。
	/// </summary>
	/// <returns>シャドウマップのリソース</returns>
	ID3D12Resource* GetShadowTexture(int32_t handle) const;

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// DSVの設定を行います。（シャドウパス用）
	/// </summary>
	/// <param name="handle">シャドウマップハンドル</param>
	void SetShadowDSV(int32_t handle);

	/// <summary>
	/// シャドウマップのクリアを行います。
	/// </summary>
	/// <param name="handle">シャドウマップハンドル</param>
	/// <param name="clearDepth">クリアする深度値</param>
	void ClearShadowMap(int32_t handle, float clearDepth = 1.0f);

	/// <summary>
	/// シャドウマップの状態遷移を行います。
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	/// <param name="handle">シャドウマップハンドル</param>
	/// <param name="newState">新しいリソースステート</param>
	void TransitionShadowResource(ID3D12GraphicsCommandList* cmdList, int32_t handle, D3D12_RESOURCE_STATES newState);

private:
	/// <summary>
	/// コピー禁止
	/// </summary>
	ShadowMapManager() = default;
	~ShadowMapManager() = default;
	ShadowMapManager(const ShadowMapManager&) = delete;
	ShadowMapManager& operator=(const ShadowMapManager&) = delete;

	/// <summary>
	/// シャドウマップ描画用PSOを生成します。
	/// </summary>
	void CreateShadowPSO();

	/// <summary>
	/// スキニング用シャドウマップ描画用PSOを生成します。
	/// </summary>
	void CreateShadowSkinnedPSO();

private:
	/// <summary>
	/// シャドウマップ用のリソースを管理する構造体
	/// </summary>
	struct ShadowResource {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;							/* シャドウマップリソース */
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};									/* DSVハンドル */
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle{};									/* SRVハンドル */
		uint32_t srvIndex = 0;														/* SRVインデックス */

		D3D12_RESOURCE_STATES currentState = D3D12_RESOURCE_STATE_DEPTH_WRITE;		/* 現在のリソースステート */
	};

	// =========================================================
	// Member Variables
	// =========================================================

	std::unordered_map<int32_t, ShadowResource> shadowResources_;				/* シャドウマップリソースマップ */
	int32_t currentIndex = 0;													/* シャドウマップハンドルインデックス */

	uint32_t dsvUseIndex_ = 1;													/* DSVインデックス管理用（0は通常のデプスバッファ用に予約） */

	Microsoft::WRL::ComPtr<ID3D12PipelineState> shadowPSO_;						/* シャドウマップ用PSO */
	Microsoft::WRL::ComPtr<ID3D12PipelineState> shadowSkinnedPSO_;				/* スキニング用シャドウマップPSO */
};
