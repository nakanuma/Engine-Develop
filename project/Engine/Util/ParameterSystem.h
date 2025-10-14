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
/// パラメーター変更前後の値を記録する構造体
/// </summary>
struct ParameterChange {
	std::string name;           // パラメーター名
	nlohmann::json beforeValue; // 変更前の値を格納
	nlohmann::json afterValue;  // 変更後の値を格納
};

/// <summary>
/// パラメーター抽象基底クラス
/// </summary>
class ParameterBase {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParameterBase(const std::string& name_) : name(name_) {}

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~ParameterBase() = default;

	/// <summary>
	/// パラメーターをImGui上で表示
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// パラメーターの値をJSONへ保存
	/// </summary>
	virtual void Save(nlohmann::json& j) const = 0;

	/// <summary>
	/// JSONからパラメーターの値を読み込み
	/// </summary>
	virtual void Load(const nlohmann::json& j) = 0;

public:
	/// <summary>
	/// パラメーター名
	/// </summary>
	std::string name;
};

/// <summary>
/// 個々のパラメータークラス
/// </summary>
template<typename T> class Parameter : public ParameterBase {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Parameter(const std::string& name, T* p, T minV, T maxV, T step, ParameterManager& manager) : ParameterBase(name), ptr(p), minV(minV), maxV(maxV), step(step), mgr(manager), beforeValue(*p) {}

	/// <summary>
	/// パラメーターをImGui上で表示（スライダー）
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

	/// <summary>
	/// パラメーターの値をJSONへ保存
	/// </summary>
	void Save(nlohmann::json& j) const override { j[name] = *ptr; }

	/// <summary>
	/// JSONからパラメーターの値を読み込み
	/// </summary>
	void Load(const nlohmann::json& j) override {
		if (j.contains(name)) {
			*ptr = j.at(name).get<T>();
			beforeValue = *ptr; // Load後の値を基準にする
		}
	}

public:
	/// <summary>
	/// パラメーターのポインタ
	/// </summary>
	T* ptr;

	/// <summary>
	/// パラメーターの最低値、最大値、ドラッグの増加量
	/// </summary>
	T minV, maxV, step;

	/// <summary>
	/// パラメーター一括管理クラス
	/// </summary>
	ParameterManager& mgr;

	/// <summary>
	/// パラメーター変更履歴（ドラッグ前の値を保持）
	/// </summary>
	T beforeValue{};
};

/// <summary>
/// 複数のパラメーターをまとめて管理
/// </summary>
class ParameterManager {
public:
	/// <summary>
	/// 新しいパラメーターを追加
	/// </summary>
	template<typename T> void Add(const std::string& name, T* ptr, T minV, T maxV, T step = (T)1) { params.emplace_back(std::make_unique<Parameter<T>>(name, ptr, minV, maxV, step, *this)); }

	/// <summary>
	/// 登録されたすべてのパラメーターをGUI上に描画
	/// </summary>
	void DrawAll();

	/// <summary>
	/// パラメーターの値をJSONへ保存
	/// </summary>
	bool SaveToFile(const std::string& filename);

	/// <summary>
	/// JSONからパラメーターの値を読み込み
	/// </summary>
	bool LoadFromFile(const std::string& filename);

	/// <summary>
	/// 変更履歴の追加（値変更時に呼び出される）
	/// </summary>
	void PushHistory(const ParameterChange& change);

	/// <summary>
	/// パラメーター変更前に戻す
	/// </summary>
	void Undo();

	/// <summary>
	/// パラメーター変更後に戻す
	/// </summary>
	void Redo();

	/// <summary>
	/// パラメーターの変更があったかどうかを確認（自動保存処理に使用）
	/// </summary>
	bool NeedsSave() const { return needsSave_; }

	/// <summary>
	/// 自動保存処理後に呼び出してフラグリセット
	/// </summary>
	void ClearNeedsSave() { needsSave_ = false; }

private:
	/// <summary>
	/// 登録された全てのパラメーター
	/// </summary>
	std::vector<std::unique_ptr<ParameterBase>> params;

	/// <summary>
	/// 操作履歴
	/// </summary>
	std::vector<ParameterChange> undoStack;
	std::vector<ParameterChange> redoStack;

	/// <summary>
	/// パラメーター変更時の自動保存フラグ
	/// </summary>
	bool needsSave_ = false;

	friend class ParameterBase;
};

/// <summary>
/// パラメーター調整を可能にするための基底クラス
/// </summary>
class IConfigurable {
protected:
	/// <summary>
	/// 調整パラメーター登録
	/// </summary>
	template<typename T> void RegisterParam(const std::string& name, T* ptr, T minV, T maxV, T step = (T)1) { mgr.Add(name, ptr, minV, maxV, step); }

public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void InitConfig();

	/// <summary>
	/// jsonの保存/読み込み先のファイルパス設定
	/// </summary>
	void SetConfigPath(const std::string& path) { subPath = path; }

	/// <summary>
	/// ウインドウを表示
	/// </summary>
	void DrawConfigWindow(const char* title);

private:
	bool SaveConfig(const std::string& path) { return mgr.SaveToFile(path); }
	bool LoadConfig(const std::string& path) { return mgr.LoadFromFile(path); }

public:
	/// <summary>
	/// パラメーター管理クラス
	/// </summary>
	ParameterManager mgr;

	/// <summary>
	/// コンフィグファイルのルートディレクトリ
	/// </summary>
	const std::string basePath = "resources/Configs/";

	/// <summary>
	/// 各派生クラスが設定する相対パス（SetConfigPath関数で設定）
	/// </summary>
	std::string subPath = "untitled.json";

	/// <summary>
	/// 初回自動読み込みフラグ
	/// </summary>
	bool isLoaded_ = false;
};