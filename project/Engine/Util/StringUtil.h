#pragma once
#include <Windows.h>
#include <format>
#include <string>

// string -> wstring
std::wstring ConvertString(const std::string& str);
// wstring -> string
std::string ConvertString(const std::wstring& str);
