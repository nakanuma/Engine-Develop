#pragma once

// ---------------------------------------------------------
// Externals Includes
// ---------------------------------------------------------
#include <externals/nlohmann/json.hpp>

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <fstream>
#include <string>
#include <type_traits>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <ImguiWrapper.h>
#include <Input/Input.h>

// ---------------------------------------------------------
// Foward Declaration
// ---------------------------------------------------------
class ParameterManager;

/// <summary>
/// パラメーター変更前後の値を記録する構造体
/// </summary>
struct ParameterChange {
	std::string name;					/* パラメーター名 */
	nlohmann::json beforeValue;			/* 変更前の値 */
	nlohmann::json afterValue;			/* 変更後の値 */
};

// =========================================================
// パラメーター抽象基底クラス
// =========================================================
class ParameterBase {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="name_">パラメーター名</param>
	ParameterBase(const std::string& name_) : name(name_) {}

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~ParameterBase() = default;

	/// <summary>
	/// パラメーターをImGui上で表示します。
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// パラメーターの値をJSONへ保存します。
	/// </summary>
	/// <param name="j">jsonファイル</param>
	virtual void Save(nlohmann::json& j) const = 0;

	/// <summary>
	/// JSONからパラメーターの値を読み込みます。
	/// </summary>
	/// <param name="j">jsonファイル</param>
	virtual void Load(const nlohmann::json& j) = 0;

public:
	// =========================================================
	// Member Variables
	// =========================================================

	std::string name;		/* パラメーター名 */
};

// =========================================================
// 個々のパラメータークラス
// =========================================================
template<typename T> class Parameter : public ParameterBase {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="name">パラメーター名</param>
	/// <param name="p">ポインタ</param>
	/// <param name="minV">最小値</param>
	/// <param name="maxV">最大値</param>
	/// <param name="step">ステップ値</param>
	/// <param name="manager">パラメーター管理クラス</param>
	Parameter(const std::string& name, T* p, T minV, T maxV, T step, ParameterManager& manager) : ParameterBase(name), ptr(p), minV(minV), maxV(maxV), step(step), mgr(manager), beforeValue(*p) {}

	/// <summary>
	/// パラメーターをImGui上で表示します。
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
	/// パラメーターの値をJSONへ保存します。
	/// </summary>
	/// <param name="j">jsonファイル</param>
	void Save(nlohmann::json& j) const override { j[name] = *ptr; }

	/// <summary>
	/// JSONからパラメーターの値を読み込みます。
	/// </summary>
	/// <param name="j">jsonファイル</param>
	void Load(const nlohmann::json& j) override {
		if (j.contains(name)) {
			*ptr = j.at(name).get<T>();
			beforeValue = *ptr; // Load後の値を基準にする
		}
	}

public:
	// =========================================================
	// Member Variables
	// =========================================================

	T* ptr;							/* パラメーターのポインタ */
	T minV, maxV, step;				/* 最小値、最大値、ステップ値 */
	ParameterManager& mgr;			/* パラメーター管理クラス */
	T beforeValue{};				/* 変更前の値 */
};

// =========================================================
// 複数のパラメーターをまとめて管理するクラス
// =========================================================
class ParameterManager {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 新しいパラメーターを追加します。
	/// </summary>
	/// <typeparam name="T">クラス</typeparam>
	/// <param name="name">パラメーター名</param>
	/// <param name="ptr">ポインタ</param>
	/// <param name="minV">最小値</param>
	/// <param name="maxV">最大値</param>
	/// <param name="step">ステップ値</param>
	template<typename T> void Add(const std::string& name, T* ptr, T minV, T maxV, T step = (T)1) { params.emplace_back(std::make_unique<Parameter<T>>(name, ptr, minV, maxV, step, *this)); }

	/// <summary>
	/// 登録されたすべてのパラメーターをGUI上に描画します。
	/// </summary>
	void DrawAll();

	/// <summary>
	/// パラメーターの値をJSONへ保存します。
	/// </summary>
	/// <param name="filename">ファイル名</param>
	/// <returns>保存に成功したらtrue</returns>
	bool SaveToFile(const std::string& filename);

	/// <summary>
	/// JSONからパラメーターの値を読み込みます。
	/// </summary>
	/// <param name="filename">ファイル名</param>
	/// <returns>読み込みに成功したらtrue</returns>
	bool LoadFromFile(const std::string& filename);

	/// <summary>
	/// 変更履歴を追加します。（値変更時に呼び出される）
	/// </summary>
	/// <param name="change">変更内容</param>
	void PushHistory(const ParameterChange& change);

	/// <summary>
	/// 変更履歴を元に戻します。
	/// </summary>
	void Undo();

	/// <summary>
	/// 変更履歴を再適用します。
	/// </summary>
	void Redo();

	/// <summary>
	/// パラメーターの変更があったかどうかを確認します。（自動保存処理に使用）
	/// </summary>
	bool NeedsSave() const { return needsSave_; }

	/// <summary>
	/// セーブフラグをクリアします。（自動保存処理に使用）
	/// </summary>
	void ClearNeedsSave() { needsSave_ = false; }

private:
	// =========================================================
	// Member Variables
	// =========================================================

	std::vector<std::unique_ptr<ParameterBase>> params;		/* 登録されたパラメーターリスト */
	std::vector<ParameterChange> undoStack;					/* 元に戻す用履歴スタック */
	std::vector<ParameterChange> redoStack;					/* やり直し用履歴スタック */

	bool needsSave_ = false;								/* 保存が必要かどうかのフラグ */

	friend class ParameterBase;								/* ParameterBaseからPushHistoryを呼び出すため */
};

// =========================================================
// パラメーター調整を可能にするためのインターフェースクラス
// =========================================================
class IConfigurable {
protected:
	/// <summary>
	/// 調整パラメーター登録
	/// </summary>
	/// <typeparam name="T">クラス</typeparam>
	/// <param name="name">パラメーター名</param>
	/// <param name="ptr">ポインタ</param>
	/// <param name="minV">最小値</param>
	/// <param name="maxV">最大値</param>
	/// <param name="step">ステップ値</param>
	template<typename T> void RegisterParam(const std::string& name, T* ptr, T minV, T maxV, T step = (T)1) { mgr.Add(name, ptr, minV, maxV, step); }

public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	void InitConfig();

	/// <summary>
	/// jsonの保存/読み込み先のファイルパス設定を行います。
	/// </summary>
	/// <param name="path">ファイルパス</param>
	void SetConfigPath(const std::string& path) { subPath = path; }

	/// <summary>
	/// ウインドウを表示します。
	/// </summary>
	/// <param name="title">ウインドウタイトル</param>
	void DrawConfigWindow(const char* title);

private:
	/// <summary>
	/// 設定をファイルに保存します。
	/// </summary>
	/// <param name="path">ファイルパス</param>
	/// <returns>成功した場合はtrue</returns>
	bool SaveConfig(const std::string& path) { return mgr.SaveToFile(path); }

	/// <summary>
	/// 設定をファイルから読み込みます。
	/// </summary>
	/// <param name="path">ファイルパス</param>
	/// <returns>成功した場合はtrue</returns>
	bool LoadConfig(const std::string& path) { return mgr.LoadFromFile(path); }

public:
	// =========================================================
	// Member Variables
	// =========================================================

	ParameterManager mgr;									/* パラメーター管理クラス */

	const std::string basePath = "resources/Configs/";		/* 基本パス */
	std::string subPath = "untitled.json";					/* サブパス */

	bool isLoaded_ = false;									/* 設定が読み込まれたかどうか */
};