#include "MyGame.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// ゲームフレームワークを生成
	std::unique_ptr<Framework> game = std::make_unique<MyGame>();
	// ゲームのメインループを開始
	game->Run();

	return 0;
}