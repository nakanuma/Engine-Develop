#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <Windows.h>
#include <format>
#include <string>

/// <summary>
/// stringをwstringに変換します。
/// </summary>
/// <param name="str">string文字列</param>
/// <returns>wstring文字列</returns>
std::wstring ConvertString(const std::string& str);

/// <summary>
/// wstringをstringに変換します。
/// </summary>
/// <param name="str">wstring文字列</param>
/// <returns>string文字列</returns>
std::string ConvertString(const std::wstring& str);
