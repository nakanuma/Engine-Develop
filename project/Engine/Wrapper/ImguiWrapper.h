#pragma once
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "MyWindow.h"
#include <string>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ImguiWrapper
{
public:
	static void Initialize(ID3D12Device* device, int bufferCount, DXGI_FORMAT rtvFormat, ID3D12DescriptorHeap* srvHeap);
	static void Finalize();
	static void NewFrame();
	static void Render(ID3D12GraphicsCommandList* commandList);
};

class ImGuiUtil {
public:
	static void ImageWindow(std::string windowName, int32_t textureHandle);
	static void DepthWindow(std::string windowName, int32_t textureHandle);
};

