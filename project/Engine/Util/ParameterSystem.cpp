#include "ParameterSystem.h"

void Cygnus::ParameterManager::DrawAll() {
	for (auto& p : params_)
		p->Draw();
}

bool Cygnus::ParameterManager::SaveToFile(const std::string& filename) {
	nlohmann::json j;

	// パラメーターをJSON形式にして保存
	for (auto& p : params_) {
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

bool Cygnus::ParameterManager::LoadFromFile(const std::string& filename) {
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
	for (auto& p : params_) {
		p->Load(j);
	}

	return true;
}

void Cygnus::ParameterManager::PushHistory(const ParameterChange& change) {
	// 新しい変更履歴をundoStackに追加
	undoStack_.push_back(change);
	// redoStackは意味をなさなくなるのでクリア
	redoStack_.clear();
	// セーブすることを知らせる
	needsSave_ = true;
}

void Cygnus::ParameterManager::Undo() {
	if (undoStack_.empty())
		return;

	// 直近の変更を取り出す
	ParameterChange change = undoStack_.back();
	undoStack_.pop_back();
	redoStack_.push_back(change);

	// 値の更新を行う
	for (auto& p : params_) {
		if (p->name_ == change.name) {
			p->Load(nlohmann::json{
			    {p->name_, change.beforeValue}
            });
		}
	}

	// セーブフラグを立てる
	needsSave_ = true;
}

void Cygnus::ParameterManager::Redo() {
	if (redoStack_.empty())
		return;

	// 直近の変更を取り出す
	ParameterChange change = redoStack_.back();
	redoStack_.pop_back();
	undoStack_.push_back(change);

	// 値の更新を行う
	for (auto& p : params_) {
		if (p->name_ == change.name) {
			p->Load(nlohmann::json{
			    {p->name_, change.afterValue}
            });
		}
	}

	// セーブフラグを立てる
	needsSave_ = true;
}

void Cygnus::Configurator::InitConfig() {
	// 初回だけ読み込み
	if (!isLoaded_) {
		LoadConfig(kBasePath + subPath_);
		isLoaded_ = true;
	}
}

void Cygnus::Configurator::DrawConfigWindow(const char* title) {
#ifdef USE_IMGUI
	if (ImGui::Begin(title)) {
		// 調整項目を全て描画
		mgr_.DrawAll();

		// パラメーター変更されたら自動保存を行う
		if (mgr_.NeedsSave()) {
			SaveConfig(kBasePath + subPath_);
			mgr_.ClearNeedsSave();
		}

		// Undo/Redoのショートカット
		if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_Z)) {
			mgr_.Undo();
		}
		if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_Y)) {
			mgr_.Redo();
		}
		// わかりやすさのためにボタン配置
		if (ImGui::Button("Undo")) {
			mgr_.Undo();
		}
		ImGui::SameLine();
		if (ImGui::Button("Redo")) {
			mgr_.Redo();
		}
	}
	ImGui::End();
#endif
}
