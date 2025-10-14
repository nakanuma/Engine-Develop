#include "ParameterSystem.h"

void ParameterManager::DrawAll() {
	for (auto& p : params)
		p->Draw();
}

bool ParameterManager::SaveToFile(const std::string& filename) {
	nlohmann::json j;

	for (auto& p : params) {
		p->Save(j);
	}

	std::ofstream ofs(filename);

	if (!ofs.is_open()) {
		return false;
	}

	ofs << j.dump(4);

	return true;
}

bool ParameterManager::LoadFromFile(const std::string& filename) {
	std::ifstream ifs(filename);

	if (!ifs.is_open()) {
		return false;
	}

	nlohmann::json j;
	ifs >> j;

	for (auto& p : params) {
		p->Load(j);
	}

	return true;
}

void ParameterManager::PushHistory(const ParameterChange& change) {
	undoStack.push_back(change);
	redoStack.clear();
	needsSave_ = true;
}

void ParameterManager::Undo() {
	if (undoStack.empty())
		return;

	// 直近の変更を取り出す
	ParameterChange change = undoStack.back();
	undoStack.pop_back();
	redoStack.push_back(change);

	// 値の更新を行う
	for (auto& p : params) {
		if (p->name == change.name) {
			p->Load(nlohmann::json{
			    {p->name, change.beforeValue}
            });
		}
	}

	// セーブフラグを立てる
	needsSave_ = true;
}

void ParameterManager::Redo() {
	if (redoStack.empty())
		return;

	// 直近の変更を取り出す
	ParameterChange change = redoStack.back();
	redoStack.pop_back();
	undoStack.push_back(change);

	// 値の更新を行う
	for (auto& p : params) {
		if (p->name == change.name) {
			p->Load(nlohmann::json{
			    {p->name, change.afterValue}
            });
		}
	}

	// セーブフラグを立てる
	needsSave_ = true;
}

void IConfigurable::InitConfig() {
	// 初回だけ読み込み
	if (!isLoaded_) {
		LoadConfig(basePath + subPath);
		isLoaded_ = true;
	}
}

void IConfigurable::DrawConfigWindow(const char* title) {
	if (ImGui::Begin(title)) {
		// 調整項目を全て描画
		mgr.DrawAll();

		// パラメーター変更されたら自動保存を行う
		if (mgr.NeedsSave()) {
			SaveConfig(basePath + subPath);
			mgr.ClearNeedsSave();
		}

		// Undo/Redoのショートカット
		if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_Z)) {
			mgr.Undo();
		}
		if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_Y)) {
			mgr.Redo();
		}
		// わかりやすさのためにボタン配置
		if (ImGui::Button("Undo")) {
			mgr.Undo();
		}
		ImGui::SameLine();
		if (ImGui::Button("Redo")) {
			mgr.Redo();
		}
	}
	ImGui::End();
}
