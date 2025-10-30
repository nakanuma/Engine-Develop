#include "Logger.h"
#include "MyWindow.h"

void Log(const std::string& message) { 
	// ANSI文字列をデバッグ出力へ送信
	OutputDebugStringA(message.c_str()); 
}

void Log(const std::wstring& message) { 
	// Unicode文字列をデバッグ出力へ送信
	OutputDebugStringW(message.c_str()); 
}
