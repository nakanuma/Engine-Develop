#pragma once

// Engine
#include <DirectXBase.h>
#include <MyMath.h>

class LineDrawer
{
public:
	static LineDrawer* GetInstance();
	void Initialize();
	void DrawLine(const Float3& start, const Float3& end, const Float4& color);

private:
	struct Vertex {
		Float3 position;
		Float4 color;
	};

	DirectXBase* dxBase_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};

private:
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

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_;
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_;
	DescriptorHeap dsvDescriptorHeap_;

	D3D12_BLEND_DESC blendDesc_; // kBlendModeNormal
};