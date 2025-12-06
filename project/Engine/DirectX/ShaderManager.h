#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <wrl.h>
#include <dxcapi.h>
#include <string>
#include <unordered_map>

namespace Cygnus {
// =========================================================
// シェーダー管理クラス
// =========================================================
class ShaderManager {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static ShaderManager* GetInstance();

	/// <summary>
	/// シェーダー管理クラスの初期化処理を行います。
	/// </summary>
	void Initialize();

	/// <summary>
	/// シェーダーファイルの読み込みを行います。
	/// </summary>
	/// <param name="name">マップに保存するシェーダー名（任意）</param>
	/// <param name="path">シェーダーファイルのパス</param>
	/// <param name="profile">シェーダープロファイル</param>
	void LoadShader(const std::string& name, const std::wstring& path, const wchar_t* profile);

	/// <summary>
	/// シェーダーの取得を行います。
	/// </summary>
	/// <param name="name">マップに保存したシェーダー名</param>
	/// <returns>シェーダーのバイナリデータ</returns>
	IDxcBlob* GetShader(const std::string& name) const;

private:
	/// <summary>
	/// コピー禁止
	/// </summary>
	ShaderManager() = default;
	~ShaderManager() = default;
	ShaderManager(const ShaderManager&) = delete;
	ShaderManager& operator=(const ShaderManager&) = delete;

	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// シェーダーのコンパイルを行います。
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <param name="profile">シェーダープロファイル</param>
	/// <returns>コンパイルされたシェーダーのバイナリデータ</returns>
	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile);

private:
	// =========================================================
	// Member Variables
	// =========================================================

	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;									/* DXCユーティリティ */
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;								/* DXCコンパイラ */
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;						/* インクルードハンドラ */

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> shaders_;		/* シェーダーマップ */
};
}