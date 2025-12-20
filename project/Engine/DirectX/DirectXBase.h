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

namespace Cygnus {
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
	void Initialize();
	// DXGIデバイス初期化
	void InitializeDXGIDevice(bool enableDebugLayer = true);
	// スワップチェーンの生成
	void CreateSwapChain();
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
	// Viewportの設定
	void SetViewport();
	// Scissorの設定
	void SetScissor();

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
	
	// スワップチェーンの取得
	IDXGISwapChain4* GetSwapChain();
	// スワップチェーン設定の取得
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc();

	friend RTVManager;
private:
	// =========================================================
	// Constants
	// =========================================================

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

	// =========================================================
	// Member Variables
	// =========================================================
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;

	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[kInputElementCount];
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;

	D3D12_BLEND_DESC blendDesc_; // kBlendModeNormal
	D3D12_BLEND_DESC blendDescNone_; // kBlendModeNone
	D3D12_BLEND_DESC blendDescAdd_; // kBlendModeAdd
	D3D12_BLEND_DESC blendDescSubtract_; // kBlendModeSubtract
	D3D12_BLEND_DESC blendDescMultiply_; // kBlendModeMultiply
	D3D12_BLEND_DESC blendDescScreen_; // kBlendModeScreen
	D3D12_BLEND_DESC blendDescAlpha_; // kBlendModeAlpha

	D3D12_RASTERIZER_DESC rasterizerDesc_;

	D3D12_VIEWPORT viewport_;
	D3D12_RECT scissorRect_;

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_;
};
}