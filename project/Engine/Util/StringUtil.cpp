#include "StringUtil.h"

#include <fstream>
#include <string>

std::wstring ConvertString(const std::string& str)
{
    if (str.empty()) {
        return std::wstring();
    }

    auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
    if (sizeNeeded == 0) {
        return std::wstring();
    }
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
    return result;
}

std::string ConvertString(const std::wstring& str)
{
    if (str.empty()) {
        return std::string();
    }

    auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
    if (sizeNeeded == 0) {
        return std::string();
    }
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
    return result;
}

std::string LoadInitialSceneName(const std::string& path) { 
    std::ifstream file(path); 
    std::string sceneName;
	if (file.is_open()) {
		std::getline(file, sceneName);
		file.close();
	} else {
        // デフォルト名
		sceneName = "TITLE";
    }
	return sceneName;
}

std::wstring LoadWindowTitle(const std::string& path) { 
    std::ifstream file(path, std::ios::binary);
    std::string utf8Str;

	if (file.is_open()) {
		std::getline(file, utf8Str);
		file.close();
	} else {
		// デフォルト名
		utf8Str = "Engine";
	}

    // UTF-8をUTF-16に変換（日本語対応）
	int wideLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
	std::wstring wideStr(wideLen, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wideStr[0], wideLen);

    // null終端を除去
	if (!wideStr.empty() && wideStr.back() == L'\0') {
		wideStr.pop_back();
    }

    return wideStr;
}