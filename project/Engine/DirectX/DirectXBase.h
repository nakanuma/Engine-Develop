#pragma once
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

// MyClass
#include "MyWindow.h"
#include "DescriptorHeap.h"
#include <DirectX/ShaderManager.h>
#include <Util/FPSController.h>

// リソースリークチェック
struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker();
public:
	static D3DResourceLeakChecker* GetInstance() {
		static D3DResourceLeakChecker ins;
		return &ins;
	}
};

class RTVManager;

class DirectXBase
{
public:
	// デストラクタ
	~DirectXBase();

	// シングルトンインスタンスの取得
	static DirectXBase* GetInstance();

	// 初期化
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

	///
	/// アクセッサ
	/// 
	
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

	// BlendMode変更用PSOのgetter
	ID3D12PipelineState* GetPipelineStateBlendModeNone() { return graphicsPipelineStateBlendModeNone_.Get(); };
	ID3D12PipelineState* GetPipelineStateBlendModeAdd() { return graphicsPipelineStateBlendModeAdd_.Get(); };
	ID3D12PipelineState* GetPipelineStateBlendModeSubtract() { return graphicsPipelineStateBlendModeSubtract_.Get(); };
	ID3D12PipelineState* GetPipelineStateBlendModeMultiply() { return graphicsPipelineStateBlendModeMultiply_.Get(); };
	ID3D12PipelineState* GetPipelineStateBlendModeScreen() { return graphicsPipelineStateBlendModeScreen_.Get(); };

	// Particle用ルートシグネチャを取得
	ID3D12RootSignature* GetRootSignatureParticle() { return rootSignatureParticle_.Get(); }
	// InstancedObject用ルートシグネチャを取得
	ID3D12RootSignature* GetRootSignatureInstancedObject() { return rootSignatureInstancedObject_.Get(); }

	// Particle用PSOを取得
	ID3D12PipelineState* GetPipelineStateParticle() { return graphicsPipelineStateParticle_.Get(); };

	// ポストエフェクト用PSOを取得
	ID3D12PipelineState* GetPipelineStateSobelFilter() { return graphicsPipelineStateSobelFilter_.Get(); }
	// Skybox用PSOを取得
	ID3D12PipelineState* GetPipelineStateSkybox() { return graphicsPipelineStateSkybox_.Get(); }
	// Skinning用PSOを取得
	ID3D12PipelineState* GetPipelineStateSkinning() { return graphicsPipelineStateSkinning_.Get(); }
	// InstancedObject用PSOを取得
	ID3D12PipelineState* GetPipelineStateInstancedObject() { return graphicsPipelineStateInstancedObject_.Get(); }

	friend RTVManager;
private:
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

	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[5];
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;

	D3D12_BLEND_DESC blendDesc_; // kBlendModeNormal
	D3D12_BLEND_DESC blendDescNone_; // kBlendModeNone
	D3D12_BLEND_DESC blendDescAdd_; // kBlendModeAdd
	D3D12_BLEND_DESC blendDescSubtract_; // kBlendModeSubtract
	D3D12_BLEND_DESC blendDescMultiply_; // kBlendModeMultiply
	D3D12_BLEND_DESC blendDescScreen_; // kBlendModeScreen
	// BlendMode変更用のPSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBlendModeNone_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBlendModeAdd_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBlendModeSubtract_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBlendModeMultiply_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateBlendModeScreen_;

	// ポストエフェクト用PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSobelFilter_;
	// Skybox用PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSkybox_;
	// Skinning用PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateSkinning_;
	// InstancedObject用PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineStateInstancedObject_;

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