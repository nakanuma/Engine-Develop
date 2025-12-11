#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <d3d12.h>
#include <wrl.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <cassert>

namespace Cygnus {
/// <summary>
/// PSO識別用の列挙型
/// </summary>
enum class PSOType {
	// 基本
	Default,			/* 通常 */

	// インスタンシング
	InstancedObject,
	InstancedObjectNone,
	InstancedObjectNormal,
	InstancedObjectAdd,
	InstancedObjectSubtract,
	InstancedObjectMultiply,
	InstancedObjectScreen,
	InstancedObjectAlpha,

	// ポストエフェクト用
	Grayscale,			/* グレースケール */
	Vignette,			/* ビネット */
	DamageVignette,     /* ダメージビネット */
	BoxFilter,			/* ボックスフィルター */
	GaussianFilter,		/* ガウシアンフィルター */
	GaussianHorizontal, /* ガウシアンフィルター（水平） */
	GaussianVertical,   /* ガウシアンフィルター（垂直） */
	RadialBlur,         /* 放射状ブラー */
	InvertColor,		/* 色反転 */
	Sepia,				/* セピア調 */
	Posterize,			/* ポスタリゼーション */
	Emboss,				/* エンボス */
	Sharpen,			/* シャープ */
	ColorAberration,	/* 色収差 */
	BarrelDistortion,	/* バレルディストーション */
	WaveDistortion,		/* 波形ディストーション */
	Pixelation,			/* ピクセル化 */
	GlitchEffect,		/* グリッチエフェクト */
	BloomExtract,       /* 明度抽出（ブルーム用） */
	SobelFilter,        /* ソベルフィルター（アウトライン用） */

	// 特殊な用途
	Skybox,				/* スカイボックス */
	Skinning,			/* スキニング */
};

// =========================================================
// パイプラインステートオブジェクト管理クラス
// =========================================================
class PipelineStateManager {
public:
	/// <summary>
	/// PSO生成用の設定構造体
	/// </summary>
	struct PSODescriptor {
		const char* vertexShaderName = nullptr;
		const char* pixelShaderName = nullptr;
		ID3D12RootSignature* rootSignature = nullptr;
		D3D12_BLEND_DESC blendDesc{};
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		D3D12_INPUT_LAYOUT_DESC inputLayout{};
		D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		DXGI_FORMAT dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	};

public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// シングルトンインスタンスの取得を行います。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static PipelineStateManager* GetInstance();

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	/// <param name="device"></param>
	/// <param name="rootSignature"></param>
	/// <param name="rootSignatureParticle"></param>
	/// <param name="rootSignatureInstancedObject"></param>
	/// <param name="inputLayout"></param>
	/// <param name="blendNormal"></param>
	/// <param name="blendNone"></param>
	/// <param name="blendAdd"></param>
	/// <param name="blendSubtract"></param>
	/// <param name="blendMultiply"></param>
	/// <param name="blendScreen"></param>
	/// <param name="blendAlpha"></param>
	/// <param name="rasterizerDesc"></param>
	/// <param name="depthStencilDesc"></param>
	void Initialize(
	    ID3D12Device* device, 
		ID3D12RootSignature* rootSignature, 
		ID3D12RootSignature* rootSignatureInstancedObject,
	    const D3D12_INPUT_LAYOUT_DESC& inputLayout, 
		const D3D12_BLEND_DESC& blendNormal, 
		const D3D12_BLEND_DESC& blendNone, 
		const D3D12_BLEND_DESC& blendAdd, 
		const D3D12_BLEND_DESC& blendSubtract,
	    const D3D12_BLEND_DESC& blendMultiply, 
		const D3D12_BLEND_DESC& blendScreen, 
		const D3D12_BLEND_DESC& blendAlpha, 
		const D3D12_RASTERIZER_DESC& rasterizerDesc,
	    const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc
	);

	/// <summary>
	/// PSOを取得します。
	/// </summary>
	/// <param name="type">取得するPSOのタイプ</param>
	/// <returns>PSO</returns>
	ID3D12PipelineState* GetPSO(PSOType type) const;

	/// <summary>
	/// 文字列キーでPSOを取得します。
	/// </summary>
	/// <param name="name">取得するPSOの文字列キー</param>
	/// <returns>PSO</returns>
	ID3D12PipelineState* GetPSOByName(const std::string& name) const;

	/// <summary>
	/// 設定を行ったPSOを登録します。
	/// </summary>
	/// <param name="name">PSO識別名</param>
	/// <param name="descriptor">PSO設定</param>
	/// <returns>生成に成功したか</returns>
	bool RegisterCustomPSO(const std::string& name, const PSODescriptor& descriptor);

	/// <summary>
	/// 登録されているPSOの数を取得します。
	/// </summary>
	/// <returns></returns>
	size_t GetPSOCount() const;

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// 全ての標準PSOを生成します。
	/// </summary>
	void CreateAllStandardPSOs();

	/// <summary>
	/// 基本的なPSOを生成します。
	/// </summary>
	void CreateBasicPSOs();

	/// <summary>
	/// インスタンシング用のPSOを生成します。
	/// </summary>
	void CreateInstancedPSOs();

	/// <summary>
	/// ポストエフェクト用のPSOを生成します。
	/// </summary>
	void CreatePostEffectPSOs();

	/// <summary>
	/// 特殊な用途のPSOを生成します。
	/// </summary>
	void CreateSpecialPSOs();

	/// <summary>
	/// PSOを生成して登録します。
	/// </summary>
	/// <param name="type">PSOタイプ</param>
	/// <param name="descriptor">PSO設定</param>
	/// <returns>生成に成功したか</returns>
	bool CreateAndRegisterPSO(PSOType type, const PSODescriptor& descriptor);

	/// <summary>
	/// PSOTypeから文字列キーを生成します。
	/// </summary>
	/// <param name="type">PSOType</param>
	/// <returns>文字列キー</returns>
	static std::string PSOTypeToString(PSOType type);

private:
	// =========================================================
	// Member Variables
	// =========================================================
	ID3D12Device* device_ = nullptr;

	// RootSignature
	ID3D12RootSignature* rootSignature_ = nullptr;
	ID3D12RootSignature* rootSignatureInstancedObject_ = nullptr;

	// デフォルト設定
	D3D12_INPUT_LAYOUT_DESC inputLayout_{};
	D3D12_BLEND_DESC blendNormal_{}; 
	D3D12_BLEND_DESC blendNone_{}; 
	D3D12_BLEND_DESC blendAdd_{};
	D3D12_BLEND_DESC blendSubtract_{};
	D3D12_BLEND_DESC blendMultiply_{};
	D3D12_BLEND_DESC blendScreen_{};
	D3D12_BLEND_DESC blendAlpha_{};
	D3D12_RASTERIZER_DESC rasterizerDesc_{};
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> psoMap_;	// PSO管理用マップ
};
}
