#include "ImguiWrapper.h"
#include <d3d12.h>
#include "TextureManager.h"
#include "RTVManager.h"

// Externals
#include <externals/nlohmann/json.hpp>

void ImguiWrapper::Initialize(ID3D12Device* device, int bufferCount, DXGI_FORMAT rtvFormat, ID3D12DescriptorHeap* srvHeap)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// フォントの変更
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
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

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = 0.0f;
	style.FrameBorderSize = 1.0f;

	ImGuiUtil::LoadImGuiStyleFromJson("resources/Configs/ImGui/imguiConfig.json");
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

	ShowMainDockSpace(); // ドックスペースの描画
}

void ImguiWrapper::Render(ID3D12GraphicsCommandList* commandList)
{
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

void ImguiWrapper::ShowMainDockSpace() { 
	ImGuiIO& io = ImGui::GetIO();
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::GetStyle().DisplaySafeAreaPadding = ImVec2(0, 0);

	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags host_window_flags = 
		ImGuiWindowFlags_NoTitleBar | 
		ImGuiWindowFlags_NoCollapse | 
		ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus | 
		ImGuiWindowFlags_NoBackground | 
		ImGuiWindowFlags_NoDocking;

	ImGui::Begin("MainDockSpaceHost", nullptr, host_window_flags);
	ImGui::PopStyleVar(3);

	ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 
		ImGuiDockNodeFlags_PassthruCentralNode
	);

	ImGui::End();
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

	ImGui::Image((SRVManager::GetInstance()->descriptorHeap.GetGPUHandle(textureHandle).ptr), finalImageSize);

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

	ImGui::Image((SRVManager::GetInstance()->descriptorHeap.GetGPUHandle(textureHandle).ptr), finalImageSize);

	ImGui::End();
}

void ImGuiUtil::SaveImGuiStyleToJson(const std::string& filepath) { 
	ImGuiStyle& style = ImGui::GetStyle(); 
	nlohmann::json j;

	for (size_t i = 0; i < ImGuiCol_COUNT; ++i) {
		ImVec4 col = style.Colors[i];
		j["Colors"][i] = {col.x, col.y, col.z, col.w};
	}

	j["WindowRounding"] = style.WindowRounding;
	j["FrameRounding"] = style.FrameRounding;
	j["GrabRounding"] = style.GrabRounding;
	j["ScrollbarRounding"] = style.ScrollbarRounding;
	j["Alpha"] = style.Alpha;

	std::ofstream file(filepath);
	if (file.is_open()) {
		file << j.dump(4);
	}
}

void ImGuiUtil::LoadImGuiStyleFromJson(const std::string& filepath) { 
	std::ifstream file(filepath);
	if (!file.is_open()) 
		return;

	nlohmann::json j;
	file >> j;

	ImGuiStyle& style = ImGui::GetStyle();

	if (j.contains("Colors")) {
		for (size_t i = 0; i < ImGuiCol_COUNT; ++i) {
			if (i < j["Colors"].size()) {
				auto& col = j["Colors"][i];
				style.Colors[i] = ImVec4(col[0], col[1], col[2], col[3]);
			}
		}
	}

	if (j.contains("WindowRounding"))
		style.WindowRounding = j["WindowRounding"];
	if (j.contains("FrameRounding"))
		style.FrameRounding = j["FrameRounding"];
	if (j.contains("GrabRounding"))
		style.GrabRounding = j["GrabRounding"];
	if (j.contains("ScrollbarRounding"))
		style.ScrollbarRounding = j["ScrollbarRounding"];
	if (j.contains("Alpha"))
		style.Alpha = j["Alpha"];
}
