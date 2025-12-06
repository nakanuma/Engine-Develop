#include "MyGame.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// ゲームフレームワークを生成
	std::unique_ptr<Cygnus::Framework> game = std::make_unique<Cygnus::MyGame>();
	// ゲームのメインループを開始
	game->Run();

	return 0;
}