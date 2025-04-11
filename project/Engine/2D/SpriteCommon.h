#pragma once
#include "DirectXBase.h"
#include "DescriptorHeap.h"

class SpriteCommon
{
public:
	// 初期化
	void Initialize(DirectXBase* dxBase);

	// 共通描画設定
	void PreDraw();

	// DxBaseのgetter
	DirectXBase* GetDxBase() const { return dxBase_; }

private:
	DirectXBase* dxBase_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの生成
	void CreateGraphicsPipeline();
	// InputLayoutの設定
	void SetInputLayout();
	// DXC初期化
	void InitializeDXC();
	// Shaderのコンパイル
	void ShaderCompile();
	// RasterizerStateの設定
	D3D12_RASTERIZER_DESC SetRasterizerState();
	// 深度バッファ生成
	void CreateDepthBuffer();
	// BlendStateの設定
	D3D12_BLEND_DESC SetBlendState();
	D3D12_BLEND_DESC SetBlendStateNone();
	D3D12_BLEND_DESC SetBlendStateAdd();
	D3D12_BLEND_DESC SetBlendStateSubtract();
	D3D12_BLEND_DESC SetBlendStateMultiply();
	D3D12_BLEND_DESC SetBlendStateScreen();

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;

	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3];
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;

	IDxcUtils* dxcUtils_;
	IDxcCompiler3* dxcCompiler_;
	IDxcIncludeHandler* includeHandler_;
	IDxcBlob* vertexShaderBlob_;
	IDxcBlob* pixelShaderBlob_;

	D3D12_RASTERIZER_DESC rasterizerDesc_;

	Microsoft::WRL::ComPtr <ID3D12Resource> depthStencilResource_;
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_;
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_;
	DescriptorHeap dsvDescriptorHeap_;



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
};

