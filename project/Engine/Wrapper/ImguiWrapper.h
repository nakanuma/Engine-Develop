#pragma once

// ---------------------------------------------------------
// Externals Includes
// ---------------------------------------------------------
#include <externals/imgui/imgui.h>
#include <externals/imgui/imgui_impl_dx12.h>
#include <externals/imgui/imgui_impl_win32.h>
#include <externals/imnodes/imnodes.h>

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <fstream>
#include <string>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <MyWindow.h>

/// <summary>
/// ウィンドウプロシージャ
/// </summary>
/// <param name="hwnd">ウィンドウハンドル</param>
/// <param name="msg">メッセージ</param>
/// <param name="wParam">ワーパラメータ</param>
/// <param name="lParam">ローパラメータ</param>
/// <returns>結果</returns>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// =========================================================
// ImGuiのラッパークラス
// =========================================================
class ImguiWrapper {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="bufferCount">バッファ数</param>
	/// <param name="rtvFormat">RTVフォーマット</param>
	/// <param name="srvHeap">SRVヒープ</param>
	static void Initialize(ID3D12Device* device, int bufferCount, DXGI_FORMAT rtvFormat, ID3D12DescriptorHeap* srvHeap);

	/// <summary>
	/// 終了処理を行います。
	/// </summary>
	static void Finalize();

	/// <summary>
	/// 新しいフレームを開始します。
	/// </summary>
	static void NewFrame();

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	static void Render(ID3D12GraphicsCommandList* commandList);

private:
	/// <summary>
	/// メインのドックスペースを表示します。
	/// </summary>
	static void ShowMainDockSpace();
};

// =========================================================
// ImGuiユーティリティクラス
// =========================================================
class ImGuiUtil {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 画像ウィンドウを表示します。
	/// </summary>
	/// <param name="windowName">ウィンドウ名</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	static void ImageWindow(std::string windowName, int32_t textureHandle);

	/// <summary>
	/// 深度ウィンドウを表示します。
	/// </summary>
	/// <param name="windowName">ウィンドウ名</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	static void DepthWindow(std::string windowName, int32_t textureHandle);

	/// <summary>
	/// ImGuiのスタイルをJSONファイルに保存します。
	/// </summary>
	/// <param name="filepath">ファイルパス</param>
	static void SaveImGuiStyleToJson(const std::string& filepath);

	/// <summary>
	/// ImGuiのスタイルをJSONファイルから読み込みます。
	/// </summary>
	/// <param name="filepath">ファイルパス</param>
	static void LoadImGuiStyleFromJson(const std::string& filepath);
};
