#include "StringUtil.h"

#include <fstream>
#include <string>

std::wstring ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	// 変換に必要なワイド文字列を取得
	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	// サイズが0の場合は失敗として空で返す
	if (sizeNeeded == 0) {
		return std::wstring();
	}

	std::wstring result(sizeNeeded, 0);
	// UTF-8 -> UTF-16に変換
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	// 変換に必要なUTF-8文字数を取得
	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	// サイズが0の場合は失敗として空で返す
	if (sizeNeeded == 0) {
		return std::string();
	}

	std::string result(sizeNeeded, 0);
	// UTF-16 -> UTF-8に変換
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}