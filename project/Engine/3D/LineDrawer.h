#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <DirectXBase.h>
#include <MyMath.h>

// =========================================================
// 線分描画ユーティリティクラス
// =========================================================
class LineDrawer {
public:
	/// <summary>
	/// 通常の線分描画に使用する頂点構造体
	/// </summary>
	struct Vertex {
		Float3 pos;						/* 座標 */
		Float4 color;					/* 色 */
	};

	/// <summary>
	/// トレーサー線の描画に使用する頂点構造体
	/// </summary>
	struct TrailVertex {
		Float3 center;					/* 線分の中心座標 */
		Float4 color;					/* 色 */
	};

	/// <summary>
	/// 頂点変換用の行列構造体
	/// </summary>
	struct TransformationMatrix {
		Matrix WVP;						/* ワールドビュー射影行列 */
	};

public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static LineDrawer* GetInstance();

	/// <summary>
	/// 線分描画クラスの初期化処理を行います。
	/// </summary>
	void Initialize();

	/// <summary>
	/// 線を追加します。
	/// </summary>
	/// <param name="start">開始位置</param>
	/// <param name="end">終了位置</param>
	/// <param name="color">色</param>
	void RegisterLine(const Float3& start, const Float3& end, const Float4& color);

	/// <summary>
	/// 扇形（塗りつぶし）を追加します。
	/// </summary>
	/// <param name="center">中心位置</param>
	/// <param name="innerRadius">内半径</param>
	/// <param name="outerRadius">外半径</param>
	/// <param name="startAngleRad">開始角度（ラジアン）</param>
	/// <param name="endAngleRad">終了角度（ラジアン）</param>
	/// <param name="segments">セグメント数</param>
	/// <param name="innerColor">内側の色</param>
	/// <param name="outerColor">外側の色</param>
	/// <param name="yOffset">Y軸オフセット</param>
	void RegisterSector(
		const Float3& center, float innerRadius, float outerRadius, float startAngleRad, float endAngleRad, uint32_t segments, const Float4& innerColor, const Float4& outerColor, float yOffset);

	/// <summary>
	/// トレーサー線を登録します。
	/// </summary>
	/// <param name="start">開始位置</param>
	/// <param name="end">終了位置</param>
	/// <param name="thickness">太さ</param>
	/// <param name="headColor">先頭の色</param>
	/// <param name="tailColor">末尾の色</param>
	void RegisterTracer(const Float3& start, const Float3& end, float thickness, const Float4& headColor, const Float4& tailColor);

	/// <summary>
	/// 線分の描画処理を行います。
	/// </summary>
	void Draw();

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// ルートシグネチャを作成します。
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// グラフィックスパイプラインを作成します。
	/// </summary>
	void CreateGraphicsPipeline();

	/// <summary>
	/// 入力レイアウトを設定します。
	/// </summary>
	void SetInputLayout();

	/// <summary>
	/// DXCの初期化を行います。
	/// </summary>
	void InitializeDXC();

	/// <summary>
	/// ラスタライザステートを設定します。
	/// </summary>
	D3D12_RASTERIZER_DESC SetRasterizerState();

	/// <summary>
	/// 深度バッファを作成します。
	/// </summary>
	void CreateDepthBuffer();

	/// <summary>
	/// ブレンドステートを設定します。
	/// </summary>
	D3D12_BLEND_DESC SetBlendState();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr size_t kConstBufferAlignment = 0xff;	/* 256バイト - 1 */

	static constexpr uint32_t kRootParameterIndexWVP = 0;	/* WVP用ルートパラメーターインデックス */
	static constexpr uint32_t kWVPShaderRegister = 0;		/* WVPのシェーダーレジスタ番号 */

	static constexpr uint32_t kNumInputElements = 2;			/* 入力要素の数 */
	static constexpr uint32_t kSemanticIndex = 0;				/* セマンティックインデックス */
	static constexpr uint32_t kInputElementIndexPositon = 0;	/* POSITION入力要素インデックス */
	static constexpr uint32_t kInputElementIndexColor = 1;		/* COLOR入力要素インデックス */

	static constexpr uint32_t kRenderTargetCount = 1; /* レンダーターゲット数 */
	static constexpr uint32_t kRenderTargetIndex = 0; /* レンダーターゲットインデックス */

	static constexpr uint32_t kSampleDescCount = 1;	/* サンプル数 */

	static constexpr uint32_t kDSVDescriptorCount = 1;	/* DSVディスクリプタヒープ数 */
	static constexpr uint32_t kDSVHeapIndex = 0;		/* DSVヒープインデックス */

	static constexpr float kTracerLengthEpsilon = 0.001f;	/* トレーサー線の長さが0に近いとみなす閾値 */
	static constexpr float kTracerThicknessHalf = 0.5f;		/* トレーサー線の太さを半分にする係数 */

	// =========================================================
	// Member Variables
	// =========================================================

	// ----- System -----
	DirectXBase* dxBase_ = nullptr;											/* DirectX基底クラス */

	// ----- Pipeline -----
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;				/* ルートシグネチャ */
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateLine_;			/* パイプラインステートオブジェクト */
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateTri_;			/* パイプラインステートオブジェクト */
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateTracer_;		/* パイプラインステートオブジェクト */

	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[kNumInputElements];			/* 頂点入力レイアウト */
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;								/* 頂点入力レイアウト構造体 */

	D3D12_RASTERIZER_DESC rasterizerDesc_;									/* ラスタライザステート */

	// DepthStencil
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;			/* 深度バッファリソース */
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_;								/* 深度ステンシルステート */
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_;									/* 深度ステンシルビュー設定構造体 */
	DescriptorHeap dsvDescriptorHeap_;										/* 深度ステンシルビュー用デスクリプタヒープ */

	D3D12_BLEND_DESC blendDesc_;											/* ブレンドステート */

	// ----- Shader -----
	IDxcUtils* dxcUtils_ = nullptr;											/* DXCユーティリティ */
	IDxcCompiler3* dxcCompiler_ = nullptr;									/* DXCコンパイラ */
	IDxcIncludeHandler* includeHandler_ = nullptr;							/* インクルードハンドラ */
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob_;								/* 頂点シェーダバイトコード */
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob_;								/* ピクセルシェーダバイトコード */
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;							/* エラーバッファ */

	// ----- Resources -----
	Microsoft::WRL::ComPtr<ID3D12Resource> lineVertexResource_;				/* 頂点バッファリソース */
	D3D12_VERTEX_BUFFER_VIEW lineVBV_{};									/* 頂点バッファビュー */

	Microsoft::WRL::ComPtr<ID3D12Resource> triVertexResource_;				/* 頂点バッファリソース */
	D3D12_VERTEX_BUFFER_VIEW triVBV_{};										/* 頂点バッファビュー */

	Microsoft::WRL::ComPtr<ID3D12Resource> tracerStripResource_;			/* トレーサー用頂点バッファリソース */
	D3D12_VERTEX_BUFFER_VIEW tracerStripVBV_{};								/* 頂点バッファビュー */

	Microsoft::WRL::ComPtr<ID3D12Resource> constanceBuffer_;				/* 定数バッファ */
	TransformationMatrix* constMap_ = nullptr;								/* 定数バッファマッピング用ポインタ */

	// 蓄積された線分
	std::vector<Vertex> lineVertices_;										/* 頂点バッファ */
	std::vector<Vertex> triVertices_;										/* 頂点バッファ */
	// トレーサー用頂点バッファ
	std::vector<TrailVertex> tracerStrip_;									/* 頂点バッファ */
};
