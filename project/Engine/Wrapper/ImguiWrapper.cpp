#include "ImguiWrapper.h"
#include "RTVManager.h"
#include "TextureManager.h"
#include <d3d12.h>

// Externals
#include <externals/nlohmann/json.hpp>

void Cygnus::ImguiWrapper::Initialize(ID3D12Device* device, int bufferCount, DXGI_FORMAT rtvFormat, ID3D12DescriptorHeap* srvHeap) {
#ifdef USE_IMGUI
	// ImGuiのバージョンチェック + コンテキスト作成
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// imnodesのコンテキスト作成（ノードエディタ用）
	ImNodes::CreateContext();

	// フォントの変更
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(kFontFilePath, kFontSize);

	// ドッキング機能有効
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// フォントテクスチャを取得
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	// ダークテーマを適用
	ImGui::StyleColorsDark();

	// Win32 & DX12用に初期化
	ImGui_ImplWin32_Init(Window::GetHandle());
	ImGui_ImplDX12_Init(device, bufferCount, rtvFormat, srvHeap, srvHeap->GetCPUDescriptorHandleForHeapStart(), srvHeap->GetGPUDescriptorHandleForHeapStart());

	// ウィンドウのスタイル調整
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = kWindowBorderSize;
	style.FrameBorderSize = kFrameBorderSize;

	// JSONからスタイル設定を読み込み
	ImGuiUtil::LoadImGuiStyleFromJson(kImGuiConfigPath);

#endif
}

void Cygnus::ImguiWrapper::Finalize() {
#ifdef USE_IMGUI
	// imnodesコンテキスト破棄
	ImNodes::DestroyContext();

	// DX12 / Win32 のバインディング終了
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();

	// ImGuiコンテキスト破棄
	ImGui::DestroyContext();
#endif
}

void Cygnus::ImguiWrapper::NewFrame() {
#ifdef USE_IMGUI
	// DX12 / Win32用フレーム処理
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// メインドックスペース描画
	ShowMainDockSpace();
#endif
}

void Cygnus::ImguiWrapper::Render(ID3D12GraphicsCommandList* commandList) {
#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif
}

void Cygnus::ImguiWrapper::ShowMainDockSpace() {
#ifdef USE_IMGUI
	ImGuiIO& io = ImGui::GetIO();
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	// ウィンドウスタイルを調整
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, kDockWindowPadding);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, kDockWindowRounding);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, kDockWindowBorderSize);
	ImGui::GetStyle().DisplaySafeAreaPadding = kDisplaySafeAreaPadding;

	// ウィンドウ位置・サイズをビューポートに合わせる
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	// ウィンドウフラグ設定
	ImGuiWindowFlags host_window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
	                                     ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking;

	ImGui::Begin("MainDockSpaceHost", nullptr, host_window_flags);
	ImGui::PopStyleVar(3);

	// ドックスペース作成
	ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockspace_id, kDockSpaceSize, ImGuiDockNodeFlags_PassthruCentralNode);

	ImGui::End();
#endif
}

void Cygnus::ImGuiUtil::ImageWindow(const std::string& windowName, int32_t textureHandle) {
#ifdef USE_IMGUI
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
	ImVec2 topLeft = {(wndSize.x - finalImageSize.x) * kImageCenterScale + cntRegionMin.x, (wndSize.y - finalImageSize.y) * kImageCenterScale + cntRegionMin.y};
	ImGui::SetCursorPos(topLeft);

	ImGui::Image((SRVManager::GetInstance()->descriptorHeap_.GetGPUHandle(textureHandle).ptr), finalImageSize);

	ImGui::End();
#endif
}

void Cygnus::ImGuiUtil::SaveImGuiStyleToJson(const std::string& filepath) {
#ifdef USE_IMGUI
	ImGuiStyle& style = ImGui::GetStyle();
	nlohmann::json j;

	// 色情報を保存
	for (size_t i = 0; i < ImGuiCol_COUNT; ++i) {
		ImVec4 col = style.Colors[i];
		j["Colors"][i] = {col.x, col.y, col.z, col.w};
	}

	// 各種スタイルパラメーターを保存
	j["WindowRounding"] = style.WindowRounding;
	j["FrameRounding"] = style.FrameRounding;
	j["GrabRounding"] = style.GrabRounding;
	j["ScrollbarRounding"] = style.ScrollbarRounding;
	j["Alpha"] = style.Alpha;

	// JSONファイルに書き込み
	std::ofstream file(filepath);
	if (file.is_open()) {
		file << j.dump(4);
	}
#endif
}

void Cygnus::ImGuiUtil::LoadImGuiStyleFromJson(const std::string& filepath) {
#ifdef USE_IMGUI
	// ファイルを開く
	std::ifstream file(filepath);
	// ファイルを開けばければ終了
	if (!file.is_open())
		return;

	nlohmann::json j;
	file >> j;

	// ImGuiスタイル取得
	ImGuiStyle& style = ImGui::GetStyle();

	// 色の設定を読み込み
	if (j.contains("Colors")) {
		for (size_t i = 0; i < ImGuiCol_COUNT; ++i) {
			if (i < j["Colors"].size()) {
				auto& col = j["Colors"][i];
				style.Colors[i] = ImVec4(col[0], col[1], col[2], col[3]);
			}
		}
	}

	// その他のスタイルパラメーターを読み込み
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
#endif
}
