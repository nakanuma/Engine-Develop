#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <cstdint>
#include <dxcapi.h>
#include <dxgidebug.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "dxguid.lib")

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include "MyWindow.h"
#include "DescriptorHeap.h"
#include <DirectX/ShaderManager.h>
#include <Util/FPSController.h>

/// <summary>
/// リソースリーク検出用ユーティリティクラス
/// </summary>
struct D3DResourceLeakChecker {
	/// <summary>
	/// デストラクタ
	/// </summary>
	~D3DResourceLeakChecker();
public:
	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static D3DResourceLeakChecker* GetInstance() {
		static D3DResourceLeakChecker ins;
		return &ins;
	}
};

// ---------------------------------------------------------
// Foward Declaration
// ---------------------------------------------------------
class RTVManager;

// =========================================================
// DirectX基盤クラス
// Todo : 現状は責任が大きすぎるので分割必須
// =========================================================
class DirectXBase
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DirectXBase();

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static DirectXBase* GetInstance();

	/// <summary>
	/// DirectX基盤の初期化処理を行います。
	/// </summary>
	void Initialize()
	{
		// FPS固定初期化
		FPSController::GetInstance()->InitializeFixFPS();

		// DXGIデバイス初期化
		InitializeDXGIDevice();

		// コマンド関連初期化
		InitializeCommand();

		// スワップチェーンの生成
		CreateSwapChain();

		// レンダーターゲット生成
		CreateFinalRenderTargets();

		// 深度バッファ生成
		CreateDepthBuffer();

		// フェンス生成
		CreateFence();

		// RootSignature生成
		CreateRootSignature();
		// RootSignature生成(Particle用)
		CreateRootSignatureParticle();
		// RootSignature生成（InstancedObject用）
		CreateRootSignatureInstancedObject();

		// InputLayoutの設定
		SetInputLayout();

		// BlendStateの設定
		SetBlendState();
		SetBlendStateNone();
		SetBlendStateAdd();
		SetBlendStateSubtract();
		SetBlendStateMultiply();
		SetBlendStateScreen();
		SetBlendStateAlpha();

		// RasterizerStateの設定
		SetRasterizerState();

		// ShaderManagerの初期化（PSO作成前に必ず行う）
		ShaderManager::GetInstance()->Initialize();

		// PipelineStateObjectの生成
		CreatePipelineStateObject();

		// Viewportの設定
		SetViewport();

		// Scissorの設定
		SetScissor();
	}

	// DXGIデバイス初期化
	void InitializeDXGIDevice(bool enableDebugLayer = true);
	// コマンド関連初期化
	void InitializeCommand();
	// スワップチェーンの生成
	void CreateSwapChain();
	// レンダーターゲット生成
	void CreateFinalRenderTargets();
	// フェンス生成
	void CreateFence();
	// RootSignature生成
	void CreateRootSignature();
	// RootSignature生成(Particle用)
	void CreateRootSignatureParticle();
	// RootSignature生成（InstancedObject用）
	void CreateRootSignatureInstancedObject();
	// InputLayoutの設定
	void SetInputLayout();
	// BlendStateの設定
	D3D12_BLEND_DESC SetBlendState();
	D3D12_BLEND_DESC SetBlendStateNone();
	D3D12_BLEND_DESC SetBlendStateAdd();
	D3D12_BLEND_DESC SetBlendStateSubtract();
	D3D12_BLEND_DESC SetBlendStateMultiply();
	D3D12_BLEND_DESC SetBlendStateScreen();
	D3D12_BLEND_DESC SetBlendStateAlpha();
	// RasterizerStateの設定
	D3D12_RASTERIZER_DESC SetRasterizerState();
	// PSO生成
	void CreatePipelineStateObject();
	// Viewportの設定
	void SetViewport();
	// Scissorの設定
	void SetScissor();
	// 深度バッファ生成
	void CreateDepthBuffer();

	// フレーム開始処理
	void BeginFrame();
	// フレーム終了処理
	void EndFrame();

	// 描画前処理
	void PreDraw();
	// 描画後処理
	void PostDraw();

	// =========================================================
	// Getter / Setter
	// =========================================================
	
	// デバイスの取得
	ID3D12Device* GetDevice();
	// コマンドリストの取得
	ID3D12GraphicsCommandList* GetCommandList();

	IDXGISwapChain4* GetSwapChain();
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc();
	DescriptorHeap* GetRTVHeap();
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc();
	ID3D12PipelineState* GetPipelineState();
	ID3D12PipelineState* GetPipelineStateOutline();
	ID3D12PipelineState* GetPipelineStateNoCulling();
	DescriptorHeap* GetDSVHeap();

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(UINT index);

	// BlendMode変更用PSOのgetter
	ID3D12PipelineState* GetPipelineStateBlendModeNone() { return graphicsPipelineStateBlendModeNone_.Get(); };
	ID3D12PipelineState* GetPipelineStateBlendModeAdd() { return graphicsPipelineStateBlendModeAdd_.Get(); };
	ID3D12PipelineState* GetPipelineStateBlendModeSubtract() { return graphicsPipelineStateBlendModeSubtract_.Get(); };
	ID3D12PipelineState* GetPipelineStateBlendModeMultiply() { return graphicsPipelineStateBlendModeMultiply_.Get(); };
	ID3D12PipelineState* GetPipelineStateBlendModeScreen() { return graphicsPipelineStateBlendModeScreen_.Get(); };

	// 通常ルートシグネチャを取得
	ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }
	// Particle用ルートシグネチャを取得
	ID3D12RootSignature* GetRootSignatureParticle() { return rootSignatureParticle_.Get(); }
	// InstancedObject用ルートシグネチャを取得
	ID3D12RootSignature* GetRootSignatureInstancedObject() { return rootSignatureInstancedObject_.Get(); }

	// Particle用PSOを取得
	ID3D12PipelineState* GetPipelineStateParticle() { return graphicsPipelineStateParticle_.Get(); };

	// Zバッファ無効用PSOを取得
	ID3D12PipelineState* GetPipelineStateDisableZBuffer() { return graphicsPipelineStateDisableZBuffer_.Get(); }

	// ポストエフェクト用PSOを取得
	ID3D12PipelineState* GetPipelineStateSobelFilter() { return graphicsPipelineStateSobelFilter_.Get(); } // Outline（SobelFilter）
	ID3D12PipelineState* GetPipelineStateGrayscale() { return graphicsPipelineStateGrayscale_.Get(); } // Grayscale
	ID3D12PipelineState* GetPipelineStateVignette() { return graphicsPipelineStateVignette_.Get(); } // Vignette
	ID3D12PipelineState* GetPipelineStateBoxFilter() { return graphicsPipelineStateBoxFilter_.Get(); } // Smooting（BoxFilter）
	ID3D12PipelineState* GetPipelineStateGaussianFilter() { return graphicsPipelineStateGaussianFilter_.Get(); } // Smooting（GaussianFilter）
	ID3D12PipelineState* GetPipelineStateRadialBlur() { return graphicsPipelineStateRadialBlur_.Get(); }
	ID3D12PipelineState* GetPipelineStateBloomExtract() { return graphicsPipelineStateBloomExtract_.Get(); }
	ID3D12PipelineState* GetPipelineStateInvertColor() { return graphicsPipelineStateInvertColor_.Get(); }
	ID3D12PipelineState* GetPipelineStateSepia() { return graphicsPipelineStateSepia_.Get(); }
	ID3D12PipelineState* GetPipelineStatePosterize() { return graphicsPipelineStatePosterize_.Get(); }
	ID3D12PipelineState* GetPipelineStateEmboss() { return graphicsPipelineStateEmboss_.Get(); }
	ID3D12PipelineState* GetPipelineStateSharpen() { return graphicsPipelineStateSharpen_.Get(); }
	ID3D12PipelineState* GetPipelineStateColorAberration() { return graphicsPipelineStateColorAberration_.Get(); }
	ID3D12PipelineState* GetPipelineStateBarrelDistortion() { return graphicsPipelineStateBarrelDistortion_.Get(); }
	ID3D12PipelineState* GetPipelineStateWaveDistortion() { return graphicsPipelineStateWaveDistortion_.Get(); }
	ID3D12PipelineState* GetPipelineStatePixelation() { return graphicsPipelineStatePixelation_.Get(); }
	ID3D12PipelineState* GetPipelineStateGlitchEffect() { return graphicsPipelineStateGlitchEffect_.Get(); }

	// Skybox用PSOを取得
	ID3D12PipelineState* GetPipelineStateSkybox() { return graphicsPipelineStateSkybox_.Get(); }
	// Skinning用PSOを取得
	ID3D12PipelineState* GetPipelineStateSkinning() { return graphicsPipelineStateSkinning_.Get(); }
	// InstancedObject用PSOを取得
	ID3D12PipelineState* GetPipelineStateInstancedObject() { return graphicsPipelineStateInstancedObject_.Get(); }
	// InstancedObject用PSO（各BlendMode）を取得
	ID3D12PipelineState* GetPipelineStateInstancedObjectNone() { return graphicsPipelineStateInstancedObjectNone_.Get(); }
	ID3D12PipelineState* GetPipelineStateInstancedObjectNormal() { return graphicsPipelineStateInstancedObjectNormal_.Get(); }
	ID3D12PipelineState* GetPipelineStateInstancedObjectAdd() { return graphicsPipelineStateInstancedObjectAdd_.Get(); }
	ID3D12PipelineState* GetPipelineStateInstancedObjectSubtract() { return graphicsPipelineStateInstancedObjectSubtract_.Get(); }
	ID3D12PipelineState* GetPipelineStateInstancedObjectMultiply() { return graphicsPipelineStateInstancedObjectMultiply_.Get(); }
	ID3D12PipelineState* GetPipelineStateInstancedObjectScreen() { return graphicsPipelineStateInstancedObjectScreen_.Get(); }
	ID3D12PipelineState* GetPipelineStateInstancedObjectAlpha() { return graphicsPipelineStateInstancedObjectAlpha_.Get(); }

	friend RTVManager;
private:
	// =========================================================
	// Constants
	// =========================================================

	// RootSignatureCounts
	static constexpr uint32_t kDescriptorRangeCount = 2;	/* ディスクリプタレンジの数 */
	static constexpr uint32_t kRootParameterCount = 14;		/* ルートパラメーターの数 */
	static constexpr uint32_t kStaticSamplerCount = 2;		/* スタティックサンプラーの数 */

	// RootSignatureCounts - Particle
	static constexpr uint32_t kParticleRootParameterCount = 5;	/* ルートパラメーターの数 */
	static constexpr uint32_t kParticleStaticSamplerCount = 1;	/* スタティックサンプラーの数 */

	// RootSignatureCounts - InstancedObject
	static constexpr uint32_t kInstancedObjectDescriptorRangeCount = 2;		/* ディスクリプタレンジの数 */
	static constexpr uint32_t kInstancedObjectRootParameterCount = 14;		/* ルートパラメーターの数 */
	static constexpr uint32_t kInstancedObjectStaticSamplerCount = 2;		/* スタティックサンプラーの数 */

	static constexpr uint32_t kMaterialCBVRegister = 0;			/* マテリアル用レジスタ番号 */
	static constexpr uint32_t kTransformCBVRegister = 0;		/* 変換行列用レジスタ番号 */
	static constexpr uint32_t kDirectionalLightCBVRegister = 1;	/* 平行光源用レジスタ番号 */
	static constexpr uint32_t kCameraCBVRegister = 2;			/* カメラ用レジスタ番号 */
	static constexpr uint32_t kPointLightCBVRegister = 3;		/* 点光源用レジスタ番号 */
	static constexpr uint32_t kSpotLightCBVRegister = 4;		/* スポットライト用レジスタ番号 */
	static constexpr uint32_t kWaveDistortionCBVRegister = 5;	/* 波用レジスタ番号 */
	static constexpr uint32_t kWaveGlitchEffectCBVRegister = 6;	/* グリッチエフェクト用レジスタ番号 */
	static constexpr uint32_t kLightViewProjCBVRegister = 7;	/* ライト行列用レジスタ番号 */

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

	static constexpr uint32_t kNormalSamplerRegister = 0;	/* 通常テクスチャ用サンプラーのインデックス */
	static constexpr uint32_t kShadowSamplerRegister = 1;	/* シャドウマップテクスチャ用サンプラーのインデックス */

	// InputLayout
	static constexpr uint32_t kInputElementCount = 5;			/* 入力要素の数 */
	static constexpr uint32_t kInputElementIndexPositon = 0;	/* POSITION入力要素インデックス */
	static constexpr uint32_t kInputElementIndexTexcoord = 1;	/* TEXCOORD入力要素インデックス */
	static constexpr uint32_t kInputElementIndexNormal = 2;		/* NORMAL入力要素インデックス */
	static constexpr uint32_t kInputElementIndexWeight = 3;		/* WEIGHT入力要素インデックス */
	static constexpr uint32_t kInputElementIndex = 4;			/* INDEX入力要素インデックス */

	// BlendState
	static constexpr uint32_t kRenderTargetIndex = 0;	/* ブレンド設定を行うレンダーターゲットのインデックス */

	// Viewport
	static constexpr float kViewportMinDepth = 0.0f;	/* ビューポートの最小深度 */
	static constexpr float kViewportMaxDepth = 1.0f;	/* ビューポートの最大深度 */

	static constexpr uint32_t kDefaultDSVHeapSize = 128;	/* DSVヒープのデフォルトサイズ */

	static constexpr float kDefaultClearColor[4] = {0.1f, 0.25f, 0.5f, 1.0f};	/* デフォルトの画面クリアカラー */

	static constexpr size_t kBackBufferCount = 2;	/* バックバッファの数 */

	// =========================================================
	// Member Variables
	// =========================================================
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;
	DescriptorHeap rtvDescriptorHeap_;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2];
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	D3D12_RESOURCE_BARRIER barrier_;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	uint64_t fenceValue_;
	HANDLE fenceEvent_;
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignatureParticle_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignatureInstancedObject_;

	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[kInputElementCount];
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;

	D3D12_BLEND_DESC blendDesc_; // kBlendModeNormal
	D3D12_BLEND_DESC blendDescNone_; // kBlendModeNone
	D3D12_BLEND_DESC blendDescAdd_; // kBlendModeAdd
	D3D12_BLEND_DESC blendDescSubtract_; // kBlendModeSubtract
	D3D12_BLEND_DESC blendDescMultiply_; // kBlendModeMultiply
	D3D12_BLEND_DESC blendDescScreen_; // kBlendModeScreen
	D3D12_BLEND_DESC blendDescAlpha_; // kBlendModeAlpha
	// BlendMode変更用のPSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBlendModeNone_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBlendModeAdd_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBlendModeSubtract_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBlendModeMultiply_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBlendModeScreen_;

	// Zバッファ無効パイプライン
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateDisableZBuffer_;

	// ポストエフェクト用PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSobelFilter_; // Outline（SobelFilter）
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateGrayscale_; // Grayscale
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateVignette_;   // Vignette
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBoxFilter_;  // Smooting（BoxFilter）
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateGaussianFilter_;  // Smooting（GaussianFilter）
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateRadialBlur_; // RadialBlur
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBloomExtract_; // BloomExtract
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateInvertColor_; // InvertColor
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSepia_;  // Sepia
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStatePosterize_; // Posterize
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateEmboss_; // Emboss
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSharpen_; // Sharpen
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateColorAberration_; // Aberration
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBarrelDistortion_; // BarrelDistortion
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateWaveDistortion_; // WaveDistortion
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStatePixelation_; // Pixelation
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateGlitchEffect_; // GlitchEffect

	// Skybox用PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSkybox_;
	// Skinning用PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSkinning_;
	// InstancedObject用PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateInstancedObject_;
	// InstancedObject用PSO（各BlendMode）
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateInstancedObjectNone_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateInstancedObjectNormal_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateInstancedObjectAdd_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateInstancedObjectSubtract_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateInstancedObjectMultiply_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateInstancedObjectScreen_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateInstancedObjectAlpha_;

	D3D12_RASTERIZER_DESC rasterizerDesc_;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateOutline_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateNoCulling_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateParticle_;
	D3D12_VIEWPORT viewport_;
	D3D12_RECT scissorRect_;
	Microsoft::WRL::ComPtr <ID3D12Resource> depthStencilResource_;
	DescriptorHeap dsvDescriptorHeap_;
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_;
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_;
};