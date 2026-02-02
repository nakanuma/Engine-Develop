#include "MyWindow.h"
#include "ImguiWrapper.h"

#include <DirectXBase.h>

#pragma comment(lib, "winmm.lib")

void Cygnus::Window::Create(LPCWSTR windowTitle, uint32_t width, uint32_t height) {
	// システムタイマーの分解能を上げる
	timeBeginPeriod(kTimeBeginPeriodValue);

	// ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	// ウィンドウクラス名
	wc.lpszClassName = windowTitle;
	// インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録する
	RegisterClass(&wc);

	// クライアント領域のサイズ
	winWidth = width;
	winHeight = height;

	// ウィンドウサイズを表す構造体にクライアント領域をいれる
	RECT wrc = {0, 0, static_cast<LONG>(winWidth), static_cast<LONG>(winHeight)};

	// クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの生成
	hwnd = CreateWindow(wc.lpszClassName, windowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, wrc.right - wrc.left, wrc.bottom - wrc.top, nullptr, nullptr, wc.hInstance, nullptr);

	// ウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);
}

bool Cygnus::Window::ProcessMessage() {
	MSG msg{};

	// ウィンドウの×ボタンが押されるまでループ
	// Windowにメッセージが来てたら最優先で処理させる
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.message == WM_QUIT;
}

uint32_t const Cygnus::Window::GetWidth() { return winWidth; }

uint32_t const Cygnus::Window::GetHeight() { return winHeight; }

HWND Cygnus::Window::GetHandle() { return hwnd; }

HINSTANCE Cygnus::Window::GetHInstance() { return wc.hInstance; }

LRESULT Cygnus::Window::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
#ifdef USE_IMGUI
	// ImGuiでのマウスの操作を可能にする
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
#endif

	// メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
	case WM_SYSCOMMAND:
		// Altキー単体押しでメニューモードに入るのを防ぐ
		if((wparam & 0xFFF0) == SC_KEYMENU){
			return 0;
		}
		break;

	case WM_KEYDOWN:
		// F11で切り替え
		if((wparam == VK_F11)) {
			ToggleFullscreen();
			return 0;
		}
		break;

	case WM_SIZE:
		// バックバッファのリサイズ処理を行う

		return 0;

		// ウィンドウが破壊された
	case WM_DESTROY:
		// OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void Cygnus::Window::ToggleFullscreen()
{
	isFullscreen = !isFullscreen;

	if(isFullscreen) {
		// 現在のウィンドウ位置を保存
		GetWindowRect(hwnd, &windowRect);

		// スタイルを枠無しに変更
		SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);

		// モニターの情報を取得
		HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo = {sizeof(monitorInfo)};
		GetMonitorInfo(hMonitor, &monitorInfo);

		// 画面全体にリサイズ
		SetWindowPos(hwnd, HWND_TOP,
			monitorInfo.rcMonitor.left,
			monitorInfo.rcMonitor.top,
			monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
			monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
	} else {
		// スタイルを標準に戻す
		SetWindowLongPtr(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);

		// 保存していた位置・サイズに復元
		SetWindowPos(hwnd, HWND_TOP,
			windowRect.left,
			windowRect.top,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
	}
}
