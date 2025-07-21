#pragma once

// C++
#include <fstream>
#include <string>
#include <type_traits>

// Engine
#include <ImguiWrapper.h>
#include <Input/Input.h>

// Externals
#include <externals/nlohmann/json.hpp>

// 前方宣言
class ParameterManager;

/// <summary>
/// 値変更履歴
/// </summary>
struct ParameterChange {
	std::string name;
	nlohmann::json beforeValue;
	nlohmann::json afterValue;
};

/// <summary>
/// パラメーター抽象基底クラス
/// </summary>
class ParameterBase {
public:
	std::string name; // パラメーター名
	ParameterBase(const std::string& name_) : name(name_) {}
	virtual ~ParameterBase() = default;

	virtual void Draw() = 0;
	virtual void Save(nlohmann::json& j) const = 0; // パラメーター保存
	virtual void Load(const nlohmann::json& j) = 0; // パラメーター読み込み
};

/// <summary>
/// 個々のパラメータークラス
/// </summary>
template<typename T> class Parameter : public ParameterBase {
public:
	T* ptr;             // パラメーターのポインタ
	T minV, maxV, step; // パラメーターの最低値、最大値、ドラッグの増加量
	ParameterManager& mgr;
	T beforeValue{}; // 直前の値を保持（履歴登録用）

	Parameter(const std::string& name, T* p, T minV, T maxV, T step, ParameterManager& manager) : ParameterBase(name), ptr(p), minV(minV), maxV(maxV), step(step), mgr(manager), beforeValue(*p) {}

	/// <summary>
	/// スライダー表示
	/// </summary>
	void Draw() override {
		bool changed = false;

		if constexpr (std::is_integral<T>::value) {
			int tmp = static_cast<int>(*ptr);
			changed = ImGui::DragInt(name.c_str(), &tmp, (float)step, (int)minV, (int)maxV);
			if (changed) {
				*ptr = static_cast<T>(tmp);
			}
		} else {
			float tmp = static_cast<float>(*ptr);
			changed = ImGui::DragFloat(name.c_str(), &tmp, (float)step, (float)minV, (float)maxV);
			if (changed) {
				*ptr = static_cast<T>(tmp);
			}
		}

		// ドラッグ解除後に変更があれば履歴に追加
		if (ImGui::IsItemDeactivatedAfterEdit()) {
			if (*ptr != beforeValue) {
				mgr.PushHistory(ParameterChange{name, beforeValue, *ptr});
			}
			beforeValue = *ptr; // 更新
		}
	}

	void Save(nlohmann::json& j) const override { j[name] = *ptr; }

	void Load(const nlohmann::json& j) override {
		if (j.contains(name)) {
			*ptr = j.at(name).get<T>();
			beforeValue = *ptr; // Load後の値を基準にする
		}
	}
};

/// <summary>
/// 複数のパラメーターをまとめて管理
/// </summary>
class ParameterManager {
public:
	/// <summary>
	/// パラメーター追加
	/// </summary>
	template<typename T> void Add(const std::string& name, T* ptr, T minV, T maxV, T step = (T)1) { params.emplace_back(std::make_unique<Parameter<T>>(name, ptr, minV, maxV, step, *this)); }

	void DrawAll() {
		for (auto& p : params)
			p->Draw();
	}

	bool SaveToFile(const std::string& filename);
	bool LoadFromFile(const std::string& filename);

	void PushHistory(const ParameterChange& change);
	void Undo();
	void Redo();

	bool NeedsSave() const { return needsSave_; } // セーブを行うかどうかを確認
	void ClearNeedsSave() { needsSave_ = false; } // セーブするかの状態をクリア

private:
	// パラメーターを一括管理
	std::vector<std::unique_ptr<ParameterBase>> params;

	std::vector<ParameterChange> undoStack;
	std::vector<ParameterChange> redoStack;

	// セーブを行うかのフラグ
	bool needsSave_ = false;

	friend class ParameterBase;
};

/// <summary>
/// パラメーター調整を可能にするための基底クラス
/// </summary>
class IConfigurable {
protected:
	ParameterManager mgr;
	const std::string basePath = "resources/Configs/"; // 共通のパス
	std::string subPath = "untitled.json";             // 固有のパス（SetConfigPath関数で設定）

	bool isLoaded_ = false; // 自動読み込みフラグ

	/// <summary>
	/// 調整パラメーター登録
	/// </summary>
	template<typename T> void RegisterParam(const std::string& name, T* ptr, T minV, T maxV, T step = (T)1) { mgr.Add(name, ptr, minV, maxV, step); }

public:
	/// <summary>
	/// jsonの保存/読み込み先のファイルパス設定
	/// </summary>
	void SetConfigPath(const std::string& path) { subPath = path; }

	/// <summary>
	/// ウインドウを表示
	/// </summary>
	void DrawConfigWindow(const char* title) {
		if (ImGui::Begin(title)) {
			// 初回だけ読み込み
			if (!isLoaded_) {
				LoadConfig(basePath + subPath);
				isLoaded_ = true;
			}

			// 調整項目を全て描画
			mgr.DrawAll();

			// パラメーター変更されたら自動保存
			if (mgr.NeedsSave()) {
				SaveConfig(basePath + subPath);
				mgr.ClearNeedsSave();
			}

			if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_Z)) {
				mgr.Undo();
			}
			if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_Y)) {
				mgr.Redo();
			}

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

private:
	bool SaveConfig(const std::string& path) { return mgr.SaveToFile(path); }
	bool LoadConfig(const std::string& path) { return mgr.LoadFromFile(path); }
};