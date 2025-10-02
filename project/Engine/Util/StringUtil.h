#pragma once
#include <Windows.h>
#include <string>
#include <format>

// string -> wstring
std::wstring ConvertString(const std::string& str);
// wstring -> string
std::string ConvertString(const std::wstring& str);

// 初期シーン名の読み込み
std::string LoadInitialSceneName(const std::string& path);
// ウィンドウ名の読み込み
std::wstring LoadWindowTitle(const std::string& path);
