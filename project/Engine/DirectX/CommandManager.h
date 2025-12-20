#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>

namespace Cygnus {
// =========================================================
// コマンド実行管理クラス
// =========================================================
class CommandManager
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// デストラクタ
	/// </summary>
	~CommandManager();

	/// <summary>
	/// シングルトンインスタンスを取得します。
	/// </summary>
	/// <returns>インスタンス</returns>
	static CommandManager* GetInstance();

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// コマンドの記録を開始します。
	/// </summary>
	void BeginRecording();

	/// <summary>
	/// コマンドの記録を終了し、GPUに実行を指示します。
	/// </summary>
	void EndRecordingAndExecute();

	/// <summary>
	/// GPUの処理完了を待機します。
	/// </summary>
	void WaitForGPU();

	/// <summary>
	/// 次のフレームの準備を行います。
	/// </summary>
	void PrepareNextFrame();

	/// <summary>
	/// コマンドリストを取得します。
	/// </summary>
	/// <returns>コマンドリスト</returns>
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }

	/// <summary>
	/// コマンドキューを取得します。
	/// </summary>
	/// <returns>コマンドキュー</returns>
	ID3D12CommandQueue* GetCommandQueue() const { return commandQueue_.Get(); }

	/// <summary>
	/// コマンドアロケーターを取得します。
	/// </summary>
	/// <returns>コマンドアロケーター</returns>
	ID3D12CommandAllocator* GetCommandAllocator() const { return commandAllocator_.Get(); }

	/// <summary>
	/// 現在のフェンス値を取得します。
	/// </summary>
	/// <returns></returns>
	uint64_t GetCurrentFenceValue() const { return fenceValue_; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// コマンドキューを生成します。
	/// </summary>
	void CreateCommandQueue();

	/// <summary>
	/// コマンドアロケーターを生成します。
	/// </summary>
	void CreateCommandAllocator();

	/// <summary>
	/// コマンドリストを生成します。
	/// </summary>
	void CreateCommandList();

	/// <summary>
	/// フェンスと同期オブジェクトを生成します。
	/// </summary>
	void CreateSyncObjects();

	/// <summary>
	/// フェンスにシグナルを送信します。
	/// </summary>
	void SignalFence();

	/// <summary>
	/// フェンスの完了を待機します。
	/// </summary>
	/// <param name="fenceValue"></param>
	void WaitForFenceValue(uint64_t fenceValue);

private:
	// =========================================================
	// Constants
	// =========================================================

	static constexpr uint64_t kInitialFenceValue = 0;

	// =========================================================
	// Member Variables
	// =========================================================

	ID3D12Device* device_ = nullptr;

	// コマンド実行関連
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

	// 同期関連
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	HANDLE fenceEvent_;
	uint64_t fenceValue_ = kInitialFenceValue;

	// 状態管理
	bool isRecording_ = false;	/* コマンド記録中フラグ */
};
}

