#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <unordered_map>
#include <stdint.h>

class ShadowMapManager final
{
public:
	/// <summary>
	/// インスタンス取得
	/// </summary>
	static ShadowMapManager* GetInstance();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// シャドウマップ生成
	/// </summary>
	int32_t CreateShadowMap(uint32_t width, uint32_t height);

	/// <summary>
	/// シャドウマップ書き込み開始
	/// </summary>
	void BeginShadowPass(uint32_t shadowMapHandle);

	/// <summary>
	/// シャドウマップ書き込み終了
	/// </summary>
	void EndShadowPass(uint32_t shadowMapHandle);

	/// <summary>
	/// SRVハンドル取得（通常描画パスで使用）
	/// </summary>
	int32_t GetShadowSRVHandle(int32_t handle);

	/// <summary>
	/// シャドウマップ描画用PSOの取得
	/// </summary>
	ID3D12PipelineState* GetShadowPSO() { return shadowPSO_.Get(); }

	/// <summary>
	/// シャドウマップ描画用PSO（Skinning）の取得
	/// </summary>
	ID3D12PipelineState* GetShadowSkinnedPSO() { return shadowSkinnedPSO_.Get(); }

	/// <summary>
	/// シャドウマップのリソース取得
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetShadowTexture(int32_t handle) const;

private:
	/// <summary>
	/// DSVのセット（シャドウパス用）
	/// </summary>
	void SetShadowDSV(int32_t handle);

	/// <summary>
	/// クリア
	/// </summary>
	void ClearShadowMap(int32_t handle, float clearDepth = 1.0f);

	/// <summary>
	/// シャドウマップの状態遷移
	/// </summary>
	void TransitionShadowResource(ID3D12GraphicsCommandList* cmdList, int32_t handle, D3D12_RESOURCE_STATES newState);

private:
	ShadowMapManager() = default;
	~ShadowMapManager() = default;
	ShadowMapManager(const ShadowMapManager&) = delete;
	ShadowMapManager& operator=(const ShadowMapManager&) = delete;

	/// <summary>
	/// シャドウマップ描画用PSOの生成
	/// </summary>
	void CreateShadowPSO();

	/// <summary>
	/// スキニング用シャドウマップ描画用PSOの生成
	/// </summary>
	void CreateShadowSkinnedPSO();

private:
	struct ShadowResource {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle{};
		uint32_t srvIndex = 0;

		D3D12_RESOURCE_STATES currentState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	};

	std::unordered_map<int32_t, ShadowResource> shadowResources_;
	int32_t currentIndex = 0;

	// DSVインデックス管理用
	uint32_t dsvUseIndex_ = 1;

	// シャドウマップ用PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> shadowPSO_;
	// スキニング用シャドウマップPSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> shadowSkinnedPSO_;
};

