#pragma once

// ---------------------------------------------------------
// Externals Includes
// ---------------------------------------------------------
#include <externals/DirectXTex/DirectXTex.h>

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <array>
#include <d3d12.h>
#include <unordered_map>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <DescriptorHeap.h>
#include <Float4.h>
#include <Logger.h>
#include <SRVManager.h>
#include <StringUtil.h>

// =========================================================
// テクスチャ管理クラス
// =========================================================
class TextureManager final {
public:
	/// <summary>
	/// テクスチャリソースと関連情報を保持する構造体
	/// </summary>
	struct TextureData {
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="srvManager">SRV管理クラス</param>
	static void Initialize(ID3D12Device* device, SRVManager* srvManager);

	/// <summary>
	/// テクスチャを読み込みます。
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <param name="device">デバイス</param>
	/// <returns>テクスチャハンドル</returns>
	static int Load(const std::string& filePath, ID3D12Device* device);

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static TextureManager& GetInstance();

	/// <summary>
	/// デスクリプタテーブルを設定します。
	/// </summary>
	/// <param name="rootParamIndex">ルートパラメータのインデックス</param>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	static void SetDescriptorTable(UINT rootParamIndex, ID3D12GraphicsCommandList* commandList, uint32_t textureHandle);

	/// <summary>
	/// テクスチャのメタデータを取得します。
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <returns>テクスチャのメタデータ</returns>
	static const DirectX::TexMetadata& GetMetaData(uint32_t textureHandle);

	/// <summary>
	/// 空のテクスチャを作成します。
	/// </summary>
	/// <param name="width">幅</param>
	/// <param name="height">高さ</param>
	/// <param name="clearColor">クリアカラー</param>
	/// <returns>テクスチャハンドル</returns>
	static int CreateEmptyTexture(uint32_t width, uint32_t height, Float4 clearColor = {0.1f, 0.25f, 0.5f, 1.0f});

	/// <summary>
	/// テクスチャリソースを取得します。
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <returns>テクスチャリソース</returns>
	static ID3D12Resource* GetResource(int textureHandle);

	/// <summary>
	/// 指定したテクスチャリソースに対してSRVを作成します。
	/// </summary>
	/// <param name="targetResource">ターゲットリソース</param>
	/// <param name="format">フォーマット</param>
	/// <returns>SRVインデックス</returns>
	static uint32_t CreateSRV(ID3D12Resource* targetResource, DXGI_FORMAT format);

	/// <summary>
	/// テクスチャのメタデータを取得します。
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>テクスチャのメタデータ</returns>
	const DirectX::TexMetadata& GetMetaData(const std::string& filePath) { return textureDatas[filePath].metadata; }

	/// <summary>
	/// SRVインデックスを取得します。
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>SRVインデックス</returns>
	uint32_t GetSRVIndex(const std::string& filePath) { return textureDatas[filePath].srvIndex; }

	/// <summary>
	/// SRVハンドル（GPU）を取得します。
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>SRVハンドル（GPU）</returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandleGPU(const std::string& filePath) { return textureDatas[filePath].srvHandleGPU; }

	/// <summary>
	/// コピー禁止
	/// </summary>
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// テクスチャを読み込みます。
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>読み込んだテクスチャ</returns>
	static DirectX::ScratchImage LoadTexture(const std::string& filePath);

	/// <summary>
	/// テクスチャリソースを作成します。
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="metadata">メタデータ</param>
	/// <param name="isRenderTarget">レンダーターゲットフラグ</param>
	/// <param name="clearColor">クリアカラー</param>
	/// <returns>作成したテクスチャリソース</returns>
	static Microsoft::WRL::ComPtr<ID3D12Resource>
	    CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata, bool isRenderTarget = false, Float4 clearColor = {0.1f, 0.25f, 0.5f, 1.0f});
	
	/// <summary>
	/// テクスチャデータをアップロードします。
	/// </summary>
	/// <param name="texture">テクスチャリソース</param>
	/// <param name="mipImages">ミップマップ画像</param>
	static void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

private:
	// =========================================================
	// Member Variables
	// =========================================================

	static const uint32_t kMaxTextureValue_ = 16384;										/* 最大テクスチャ数 */
	SRVManager* srvManager = nullptr;														/* SRV管理クラス */
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxTextureValue_> texResources;		/* テクスチャリソース配列 */
	std::array<DirectX::TexMetadata, kMaxTextureValue_> texMetadata;						/* テクスチャメタデータ配列 */
	std::unordered_map<std::string, TextureData> textureDatas;								/* テクスチャデータマップ */
};
