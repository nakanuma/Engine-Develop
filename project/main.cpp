#include "MyGame.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// ゲームフレームワークを生成
	Framework* game = new MyGame();
	// ゲームのメインループを開始
	game->Run();
	// ゲーム終了時にメモリ解放
	delete game;

	return 0;
}