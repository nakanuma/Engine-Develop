#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <d3d12.h>
#include <wrl.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

namespace Cygnus {
/// <summary>
/// RootSignature識別用の列挙型
/// </summary>
enum class RootSignatureType {
	Default,			/* 通常の3Dオブジェクト用 */
};

// =========================================================
// RootSignature管理クラス
// =========================================================
class RootSignatureManager
{
public:
	/// <summary>
	/// StaticSampler設定用の構造体
	/// </summary>
	struct StaticSamplerDescriptor {
		D3D12_FILTER filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		D3D12_TEXTURE_ADDRESS_MODE addressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		D3D12_TEXTURE_ADDRESS_MODE addressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		D3D12_TEXTURE_ADDRESS_MODE addressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		D3D12_COMPARISON_FUNC comparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		D3D12_STATIC_BORDER_COLOR borderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		UINT shaderRegister = 0;
		D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	};

	/// <summary>
	/// RootSignature生成用の設定構造体
	/// </summary>
	struct RootSignatureDescriptor {
		std::vector<D3D12_ROOT_PARAMETER> rootParameters;
		std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers;
		D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	};

public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// シングルトンインスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static RootSignatureManager* GetInstance();

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// 設定を行ったRootSignatureを登録します。
	/// </summary>
	/// <param name="name">識別名</param>
	/// <param name="descriptor">RootSignature設定</param>
	/// <returns>生成に成功したか</returns>
	bool RegisterCustomRootSignature(const std::string& name, const RootSignatureDescriptor& descriptor);

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// RootSignatureを取得します。
	/// </summary>
	/// <param name="type">取得するRootSignatureのタイプ</param>
	/// <returns></returns>
	ID3D12RootSignature* GetRootSignature(RootSignatureType type) const;

	/// <summary>
	/// 文字列キーでRootSignatureを取得します。
	/// </summary>
	/// <param name="name">取得するRootSignatureの文字列キー</param>
	/// <returns></returns>
	ID3D12RootSignature* GetRootSignatureByName(const std::string& name) const;

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// 全てのRootSignatureを生成します。
	/// </summary>
	void CreateAllRootSignatures();

	/// <summary>
	/// デフォルトのRootSignatureを生成します。
	/// </summary>
	void CreateDefaultRootSignature();

	/// <summary>
	/// RootSignatureを生成して登録します。
	/// </summary>
	/// <param name="type">RootSignatureのタイプ</param>
	/// <param name="descriptor">RootSignature設定</param>
	/// <returns>生成に成功したか</returns>
	bool CreateAndRegisterRootSignature(RootSignatureType type, const RootSignatureDescriptor& descriptor);

	/// <summary>
	/// RootSignatureTypeから文字列キーを生成します。
	/// </summary>
	/// <param name="type">RootSignatureのタイプ</param>
	/// <returns>文字列キー</returns>
	static std::string RootSignatureTypeToString(RootSignatureType type);

	/// <summary>
	/// RootSignatureDescriptorからD3D12_ROOT_SIGNATURE_DESCを構築します。
	/// </summary>
	/// <param name="descriptor">RootSignature設定</param>
	/// <returns>D3D12_ROOT_SIGNATURE_DESC</returns>
	D3D12_ROOT_SIGNATURE_DESC BuildD3D12Desc(const RootSignatureDescriptor& descriptor);

private:
	// =========================================================
	// Constants
	// =========================================================
	
	// Default用定数
	static constexpr uint32_t kDefaultDescriptorRangeCount = 2;		/* ディスクリプタレンジの数 */
	static constexpr uint32_t kDefaultRootParameterCount = 17;		/* ルートパラメーターの数 */
	static constexpr uint32_t kDefaultStaticSamplerCount = 2;		/* スタティックサンプラーの数 */

	// レジスタ番号定数
	static constexpr uint32_t kMaterialCBVRegister = 0;			/* マテリアル用レジスタ番号 */
	static constexpr uint32_t kTransformCBVRegister = 0;		/* 変換行列用レジスタ番号 */
	static constexpr uint32_t kDirectionalLightCBVRegister = 1;	/* 平行光源用レジスタ番号 */
	static constexpr uint32_t kCameraCBVRegister = 2;			/* カメラ用レジスタ番号 */
	static constexpr uint32_t kPointLightCBVRegister = 3;		/* 点光源用レジスタ番号 */
	static constexpr uint32_t kSpotLightCBVRegister = 4;		/* スポットライト用レジスタ番号 */
	static constexpr uint32_t kWaveDistortionCBVRegister = 5;	/* 波用レジスタ番号 */
	static constexpr uint32_t kWaveGlitchEffectCBVRegister = 6;	/* グリッチエフェクト用レジスタ番号 */
	static constexpr uint32_t kLightViewProjCBVRegister = 7;	/* ライト行列用レジスタ番号 */
	static constexpr uint32_t kEmissiveLightCBVRegister = 8;	/* エミッシブライト用レジスタ番号 */
	static constexpr uint32_t kAreaLightCBVRegister = 9;        /* エリアライト用レジスタ番号 */
	static constexpr uint32_t kDamageVignetteCBVRegister = 10;	/* ダメージビネット用レジスタ番号 */

	// RootParameterインデックス定数
	static constexpr uint32_t kRootParameterIndexMaterial = 0;			/* マテリアル用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexTransform = 1;			/* 変換行列用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexDescriptorTable = 2;	/* ディスクリプタテーブル用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexDirectionalLight = 3;	/* 平行光源用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexCamera = 4;			/* カメラ用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexStrcturedBuffer = 5;	/* ストラクチャードバッファ用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexPointLight = 6;		/* 点光源用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexSpotLight = 7;			/* スポットライト用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexCubeMap = 8;			/* キューブマップテクスチャ用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexWaveDistortion = 9;	/* 波用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexGlitchEffect = 10;		/* グリッチエフェクト用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexLightCamera = 11;		/* ライトカメラ用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexShadowMap = 12;		/* シャドウマップテクスチャ用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexLightViewProj = 13;	/* ライト行列用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexEmissiveLight = 14;	/* エミッシブライト用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexAreaLight = 15;        /* エリアライト用ルートパラメータインデックス */
	static constexpr uint32_t kRootParameterIndexDamageVignette = 16;	/* ダメージビネット用ルートパラメーターインデックス */

	// サンプラーレジスタ定数
	static constexpr uint32_t kNormalSamplerRegister = 0;	/* 通常テクスチャ用サンプラーのインデックス */
	static constexpr uint32_t kShadowSamplerRegister = 1;	/* シャドウマップテクスチャ用サンプラーのインデックス */

	// =========================================================
	// Member Variables
	// =========================================================
	ID3D12Device* device_ = nullptr;

	// RootSignature管理用マップ
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> rootSignatureMap_;

	// シリアライズ用の一時バッファ
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;
};
}

