#pragma once

// Engine
#include <DirectXBase.h>
#include <MyMath.h>

/// <summary>
/// 線の描画
/// </summary>
class LineDrawer
{
public:
	struct Vertex {
		Float3 pos;
		Float4 color;
	};

	struct TransformationMatrix {
		Matrix WVP;
	};

	/// <summary>
	/// インスタンスの取得
	/// </summary>
	static LineDrawer* GetInstance();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 線を追加
	/// </summary>
	void DrawLine(const Float3& start, const Float3& end, const Float4& color);

	/// <summary>
	/// 蓄積した線を描画
	/// </summary>
	void Render();

private:
	void CreateRootSignature();
	void CreateGraphicsPipeline();
	void SetInputLayout();
	void InitializeDXC();
	D3D12_RASTERIZER_DESC SetRasterizerState();
	void CreateDepthBuffer();
	D3D12_BLEND_DESC SetBlendState();

private:
	DirectXBase* dxBase_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[2];
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;

	D3D12_RASTERIZER_DESC rasterizerDesc_;

	// DepthStencil
	Microsoft::WRL::ComPtr <ID3D12Resource> depthStencilResource_;
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_;
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_;
	DescriptorHeap dsvDescriptorHeap_;

	D3D12_BLEND_DESC blendDesc_;

	// シェーダー関連
	IDxcUtils* dxcUtils_ = nullptr;
	IDxcCompiler3* dxcCompiler_ = nullptr;
	IDxcIncludeHandler* includeHandler_ = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob_;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob_;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;

	// バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> constanceBuffer_;
	TransformationMatrix* constMap_ = nullptr;

	// 蓄積された線分
	std::vector<Vertex> vertices_;
};

