#pragma once
#include "Float4.h"
#include <DirectXBase.h>
#include <memory>
#include <stdint.h>
#include <unordered_map>
#include <wrl.h>

class RTVManager final {
public:
	static RTVManager& GetInstance();

	static int32_t CreateRenderTargetTexture(uint32_t width, uint32_t height, Float4 clearColor = {0.1f, 0.25f, 0.5f, 1.0f});

	static void SetRenderTarget(int32_t textureHandle);

	static void SetRTtoBB();

	static void ResetResourceBarrier();

	static void ClearRTV(int32_t textureHandle, Float4 clearColor = {0.1f, 0.25f, 0.5f, 1.0f});

	static int32_t GetDepthSRVHandle(int32_t textureHandle);

private:
	std::unordered_map<int32_t, int32_t> rtvHandleMap;
	int32_t rtvIndex = 2;

	int32_t currentRenderTarget = -1;

	std::unordered_map<int32_t, Microsoft::WRL::ComPtr<ID3D12Resource>> dsvResourceMap;
	std::unordered_map<int32_t, int32_t> depthSRVHandleMap;
};
