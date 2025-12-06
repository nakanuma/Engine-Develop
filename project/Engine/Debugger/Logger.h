#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <string>

namespace Cygnus {
/// <summary>
/// stringの文字列を出力します。
/// </summary>
/// <param name="message">文字列（string）</param>
void Log(const std::string& message);

/// <summary>
/// wstringの文字列を出力します。
/// </summary>
/// <param name="message">文字列（wstring）</param>
void Log(const std::wstring& message);
}