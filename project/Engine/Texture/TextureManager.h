#pragma once
#include <d3d12.h>
#include "externals/DirectXTex/DirectXTex.h"
#include "StringUtil.h"
#include "Logger.h"
#include "DescriptorHeap.h"
#include <array>
#include "Float4.h"
#include "SRVManager.h"
#include <unordered_map>

class TextureManager final
{
public:
	struct TextureData {
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

public:
	static void Initialize(ID3D12Device* device, SRVManager* srvManager);

	static int Load(const std::string& filePath, ID3D12Device* device);

	static TextureManager& GetInstance();

	static void SetDescriptorTable(UINT rootParamIndex, ID3D12GraphicsCommandList* commandList, uint32_t textureHandle);

	static const DirectX::TexMetadata& GetMetaData(uint32_t textureHandle);

	static int CreateEmptyTexture(uint32_t width, uint32_t height, Float4 clearColor = {0.1f, 0.25f, 0.5f, 1.0f});

	static ID3D12Resource* GetResource(int textureHandle);

	// すでに存在するResourceにSRVを生成する
	static uint32_t CreateSRV(ID3D12Resource* targetResource, DXGI_FORMAT format);

	// メタデータの取得
	const DirectX::TexMetadata& GetMetaData(const std::string& filePath) { return textureDatas[filePath].metadata; }
	// SRVインデックスの取得
	uint32_t GetSRVIndex(const std::string& filePath) { return textureDatas[filePath].srvIndex; }
	// GPUハンドルの取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandleGPU(const std::string& filePath){return textureDatas[filePath].srvHandleGPU;}

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;
private:
	// TextureデータをCPUで読む
	static DirectX::ScratchImage LoadTexture(const std::string& filePath);
	// DirectX12のTextureResourceを作る
	static Microsoft::WRL::ComPtr<ID3D12Resource>
	    CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata, bool isRenderTarget = false, Float4 clearColor = {0.1f, 0.25f, 0.5f, 1.0f});
	// TextureResourceにデータを転送する
	static void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	static const uint32_t kMaxTextureValue_ = 128;

	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxTextureValue_> texResources;

	std::array<DirectX::TexMetadata, kMaxTextureValue_> texMetadata;

	// SRVManager
	SRVManager* srvManager = nullptr;
	// テクスチャデータ
	std::unordered_map<std::string, TextureData> textureDatas;
};

