#include "ImguiWrapper.h"
#include <d3d12.h>
#include "TextureManager.h"
#include "RTVManager.h"

void ImguiWrapper::Initialize(ID3D12Device* device, int bufferCount, DXGI_FORMAT rtvFormat, ID3D12DescriptorHeap* srvHeap)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// フォントの変更
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("resources/Fonts/FiraMono-Regular.ttf", 16.0f);

	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(Window::GetHandle());
	ImGui_ImplDX12_Init(
		device, 
		bufferCount, 
		rtvFormat, 
		srvHeap, 
		srvHeap->GetCPUDescriptorHandleForHeapStart(), 
		srvHeap->GetGPUDescriptorHandleForHeapStart()
	);
}

void ImguiWrapper::Finalize()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImguiWrapper::NewFrame()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImguiWrapper::Render(ID3D12GraphicsCommandList* commandList)
{
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

void ImGuiUtil::ImageWindow(std::string windowName, int32_t textureHandle) {
	ImGui::Begin(windowName.c_str());

	// タブ等を除いたウィンドウのサイズを取得(計算)
	ImVec2 cntRegionMax = ImGui::GetWindowContentRegionMax();
	ImVec2 cntRegionMin = ImGui::GetWindowContentRegionMin();
	ImVec2 wndSize = {cntRegionMax.x - cntRegionMin.x, cntRegionMax.y - cntRegionMin.y};

	// 元のアス比とImGuiウィンドウのアス比を比較
	float imageAspectRatio = static_cast<float>(TextureManager::GetMetaData(textureHandle).width) / static_cast<float>(TextureManager::GetMetaData(textureHandle).height);
	float innerWindowAspectRatio = wndSize.x / wndSize.y;
	ImVec2 finalImageSize = wndSize;

	// 横幅が大きかったら縦基準で画像サイズを決定
	if (imageAspectRatio <= innerWindowAspectRatio) {
		finalImageSize.x *= imageAspectRatio / innerWindowAspectRatio;
	}
	// 縦幅が大きかったら横基準で画像サイズを決定
	else {
		finalImageSize.y *= innerWindowAspectRatio / imageAspectRatio;
	}

	// 画像を中央に持ってくる
	ImVec2 topLeft = {(wndSize.x - finalImageSize.x) * 0.5f + cntRegionMin.x, (wndSize.y - finalImageSize.y) * 0.5f + cntRegionMin.y};
	ImGui::SetCursorPos(topLeft);

	ImGui::Image(reinterpret_cast<ImTextureID>(SRVManager::GetInstance()->descriptorHeap.GetGPUHandle(textureHandle).ptr), finalImageSize);

	ImGui::End();
}

void ImGuiUtil::DepthWindow(std::string windowName, int32_t textureHandle) {
	ImGui::Begin(windowName.c_str());

	// タブ等を除いたウィンドウのサイズを取得(計算)
	ImVec2 cntRegionMax = ImGui::GetWindowContentRegionMax();
	ImVec2 cntRegionMin = ImGui::GetWindowContentRegionMin();
	ImVec2 wndSize = {cntRegionMax.x - cntRegionMin.x, cntRegionMax.y - cntRegionMin.y};

	// 元のアス比とImGuiウィンドウのアス比を比較
	float imageAspectRatio = static_cast<float>(Window::GetWidth()) / static_cast<float>(Window::GetHeight());
	float innerWindowAspectRatio = wndSize.x / wndSize.y;
	ImVec2 finalImageSize = wndSize;

	// 横幅が大きかったら縦基準で画像サイズを決定
	if (imageAspectRatio <= innerWindowAspectRatio) {
		finalImageSize.x *= imageAspectRatio / innerWindowAspectRatio;
	}
	// 縦幅が大きかったら横基準で画像サイズを決定
	else {
		finalImageSize.y *= innerWindowAspectRatio / imageAspectRatio;
	}

	// 画像を中央に持ってくる
	ImVec2 topLeft = {(wndSize.x - finalImageSize.x) * 0.5f + cntRegionMin.x, (wndSize.y - finalImageSize.y) * 0.5f + cntRegionMin.y};
	ImGui::SetCursorPos(topLeft);

	ImGui::Image(reinterpret_cast<ImTextureID>(SRVManager::GetInstance()->descriptorHeap.GetGPUHandle(RTVManager::GetDepthSRVHandle(textureHandle)).ptr), finalImageSize);

	ImGui::End();
}