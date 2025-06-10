#pragma once

// C++
#include <string>
#include <type_traits>
#include <fstream>

// Engine
#include <ImguiWrapper.h>

// Externals
#include <externals/nlohmann/json.hpp>

/// <summary>
/// パラメーター抽象基底クラス
/// </summary>
class ParameterBase
{
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
template<typename T> class Parameter : public ParameterBase
{
public:
	T* ptr; // パラメーターのポインタ
	T minV, maxV, step; // パラメーターの最低値、最大値、ドラッグの増加量
	Parameter(const std::string& name, T* p, T minV, T maxV, T step = (T)1)
		: ParameterBase(name), ptr(p), minV(minV), maxV(maxV), step(step) {}

	/// <summary>
	/// スライダー表示
	/// </summary>
	void Draw() override
	{
		if constexpr (std::is_integral<T>::value)
		{
			int tmp = static_cast<int>(*ptr);
			if (ImGui::DragInt(name.c_str(), &tmp, (float)step, (int)minV, (int)maxV))
			{
				*ptr = static_cast<T>(tmp);
			}
		} else
		{
			float tmp = static_cast<float>(*ptr);
			if (ImGui::DragFloat(name.c_str(), &tmp, (float)step, (float)minV, (float)maxV))
			{
				*ptr = static_cast<T>(tmp);
			}
		}
	}

	void Save(nlohmann::json& j) const override
	{
		j[name] = *ptr;
	}

	void Load(const nlohmann::json& j) override
	{
		if (j.contains(name))
		{
			*ptr = j.at(name).get<T>();
		}
	}
};

/// <summary>
/// 複数のパラメーターをまとめて管理
/// </summary>
class ParameterManager
{
public:
	/// <summary>
	/// パラメーター追加
	/// </summary>
	template<typename T>
	void Add(const std::string& name, T* ptr, T minV, T maxV, T step = (T)1)
	{
		params.emplace_back(std::make_unique<Parameter<T>>(name, ptr, minV, maxV, step));
	}

	void DrawAll()
	{
		for (auto& p : params)
			p->Draw();
	}

	bool SaveToFile(const std::string& filename);
	bool LoadFromFile(const std::string& filename);

private:
	// パラメーターを一括管理
	std::vector<std::unique_ptr<ParameterBase>> params;
};

/// <summary>
/// パラメーター調整を可能にするための基底クラス
/// </summary>
class IConfigurable
{
protected:
	ParameterManager mgr;
	const std::string basePath = "resources/Configs/"; // 共通のパス
	std::string subPath = "untitled.json"; // 固有のパス（SetConfigPath関数で設定）

	/// <summary>
	/// 調整パラメーター登録
	/// </summary>
	template<typename T> void RegisterParam(const std::string& name, T* ptr, T minV, T maxV, T step = (T)1)
	{
		mgr.Add(name, ptr, minV, maxV, step);
	}

public:
	/// <summary>
	/// jsonの保存/読み込み先のファイルパス設定
	/// </summary>
	void SetConfigPath(const std::string& path) { subPath = path; }

	/// <summary>
	/// ウインドウを表示
	/// </summary>
	void DrawConfigWindow(const char* title)
	{
		if (ImGui::Begin(title))
		{
			// 調整項目を全て描画
			mgr.DrawAll();

			// 保存ボタン
			if (ImGui::Button("Save")) 
			{
				SaveConfig(basePath + subPath);
			}
			// 読み込みボタン
			if (ImGui::Button("Load")) 
			{
				LoadConfig(basePath + subPath);
			}
		}
		ImGui::End();
	}

private:
	bool SaveConfig(const std::string& path) { return mgr.SaveToFile(path); }
	bool LoadConfig(const std::string& path) { return mgr.LoadFromFile(path); }
};