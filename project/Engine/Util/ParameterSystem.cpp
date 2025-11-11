#include "ParameterSystem.h"

void ParameterManager::DrawAll() {
	for (auto& p : params)
		p->Draw();
}

bool ParameterManager::SaveToFile(const std::string& filename) {
	nlohmann::json j;

	// パラメーターをJSON形式にして保存
	for (auto& p : params) {
		p->Save(j);
	}

	// 出力ファイルストリームを開く
	std::ofstream ofs(filename);

	// ファイルを開けなかったらエラー
	if (!ofs.is_open()) {
		return false;
	}

	// JSONデータを整形してファイルに書き込み
	ofs << j.dump(4);

	return true;
}

bool ParameterManager::LoadFromFile(const std::string& filename) {
	// 入力ファイルストリームを開く
	std::ifstream ifs(filename);

	// ファイルを開けなかったらエラー
	if (!ifs.is_open()) {
		return false;
	}

	// JSONデータをパース
	nlohmann::json j;
	ifs >> j;

	// 各パラメーターに対して、JSONから値を復元する
	for (auto& p : params) {
		p->Load(j);
	}

	return true;
}

void ParameterManager::PushHistory(const ParameterChange& change) {
	// 新しい変更履歴をundoStackに追加
	undoStack.push_back(change);
	// redoStackは意味をなさなくなるのでクリア
	redoStack.clear();
	// セーブすることを知らせる
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

void Configurator::InitConfig() {
	// 初回だけ読み込み
	if (!isLoaded_) {
		LoadConfig(basePath + subPath);
		isLoaded_ = true;
	}
}

void Configurator::DrawConfigWindow(const char* title) {
#ifdef USE_IMGUI
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
#endif
}
