#pragma once
#include <Windows.h>
#include <string>
#include <format>

// string -> wstring
std::wstring ConvertString(const std::string& str);
// wstring -> string
std::string ConvertString(const std::wstring& str);

