#pragma once

// DirectX
#include <wrl.h>
#include <dxcapi.h>

// C++
#include <string>
#include <unordered_map>

/// <summary>
/// シェーダー管理
/// </summary>
class ShaderManager {
public:
	/// <summary>
	/// インスタンスの取得
	/// </summary>
	static ShaderManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// シェーダーの読み込みと管理
	/// </summary>
	void LoadShader(const std::string& name, const std::wstring& path, const wchar_t* profile);

	/// <summary>
	/// シェーダーの取得
	/// </summary>
	IDxcBlob* GetShader(const std::string& name) const;

private:
	ShaderManager() = default;
	~ShaderManager() = default;
	ShaderManager(const ShaderManager&) = delete;
	ShaderManager& operator=(const ShaderManager&) = delete;

	/// <summary>
	/// 内部コンパイル関数
	/// </summary>
	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile);

private:
	// DXC関連
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;

	// シェーダーの保存
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> shaders_;
};
