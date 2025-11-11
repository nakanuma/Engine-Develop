#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <cstdint>
#include <windows.h>

/// <summary>
/// WindowsAPIを用いたウインドウ管理クラス
/// </summary>
class Window {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// ウィンドウを作成します。
	/// </summary>
	/// <param name="windowTitle">ウィンドウタイトル</param>
	/// <param name="width">ウィンドウ幅</param>
	/// <param name="height">ウィンドウ高さ</param>
	static void Create(LPCWSTR windowTitle, uint32_t width, uint32_t height);

	/// <summary>
	/// メッセージを処理します。
	/// </summary>
	/// <returns></returns>
	static bool ProcessMessage();

	/// <summary>
	/// ウィンドウの幅を取得します。
	/// </summary>
	/// <returns>ウィンドウの幅</returns>
	static const uint32_t GetWidth();

	/// <summary>
	/// ウィンドウの高さを取得します。
	/// </summary>
	/// <returns>ウィンドウの高さ</returns>
	static const uint32_t GetHeight();

	/// <summary>
	/// ウィンドウハンドルを取得します。
	/// </summary>
	/// <returns>ウィンドウハンドル</returns>
	static HWND GetHandle();

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>インスタンス</returns>
	static HINSTANCE GetHInstance();

	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	/// <param name="hwnd">ウィンドウハンドル</param>
	/// <param name="msg">メッセージ</param>
	/// <param name="wparam">ワーパラメータ</param>
	/// <param name="lparam">ローパラメータ</param>
	/// <returns>結果</returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	// =========================================================
	// Member Variables
	// =========================================================

	inline static uint32_t winWidth;			/* ウィンドウ幅 */
	inline static uint32_t winHeight;			/* ウィンドウ高さ */

	inline static HWND hwnd;					/* ウィンドウハンドル */

	inline static WNDCLASS wc{};				/* ウィンドウクラス */
};
