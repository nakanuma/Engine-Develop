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
	/// シャドウマップ生成
	/// </summary>
	int32_t CreateShadowMap(uint32_t width, uint32_t height);

	/// <summary>
	/// DSVのセット（シャドウパス用）
	/// </summary>
	void SetShadowDSV(int32_t handle);

	/// <summary>
	/// SRVハンドル取得（通常描画パスで使用）
	/// </summary>
	int32_t GetShadowSRVHandle(int32_t handle);

	/// <summary>
	/// クリア
	/// </summary>
	void ClearShadowMap(int32_t handle, float clearDepth = 1.0f);

	/// <summary>
	/// シャドウマップ描画用PSOの取得
	/// </summary>
	ID3D12PipelineState* GetShadowPSO() { return shadowPSO_.Get(); }

	/// <summary>
	/// PSO初期化
	/// </summary>
	void InitializeShadowPSO();

	/// <summary>
	/// シャドウマップのリソース取得
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetShadowTexture(int32_t handle) const;

	/// <summary>
	/// シャドウマップの状態遷移
	/// </summary>
	void TransitionShadowResource(ID3D12GraphicsCommandList* cmdList, int32_t handle, D3D12_RESOURCE_STATES newState);

private:
	ShadowMapManager() = default;
	~ShadowMapManager() = default;
	ShadowMapManager(const ShadowMapManager&) = delete;
	ShadowMapManager& operator=(const ShadowMapManager&) = delete;

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
};

