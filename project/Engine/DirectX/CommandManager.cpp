#include "CommandManager.h"

// C++
#include <cassert>
#include <format>

// Engine
#include <Logger.h>

Cygnus::CommandManager::~CommandManager()
{
	// GPU処理の完了を待つ
	WaitForGPU();

	// フェンスイベントのクリーンアップ
	if (fenceEvent_){
		CloseHandle(fenceEvent_);
		fenceEvent_ = nullptr;
	}

	Log("Released CommandManager.\n");
}

Cygnus::CommandManager* Cygnus::CommandManager::GetInstance() {
	static CommandManager instance;
	return &instance;
}

void Cygnus::CommandManager::Initialize(ID3D12Device* device)
{
	device_ = device;

	// 各種オブジェクトの生成
	CreateCommandQueue();
	CreateCommandAllocator();
	CreateCommandList();
	CreateSyncObjects();

	Log("CommandManager initialized.\n");
}

void Cygnus::CommandManager::BeginRecording()
{
	// 既に記録中の場合は警告
	if(isRecording_){
		Log("Warning: BeginRecording called while already recording.\n");
		return;
	}

	isRecording_ = true;
}

void Cygnus::CommandManager::EndRecordingAndExecute()
{
	// 記録中でない場合は警告
	if(!isRecording_){
		Log("Warning: EndRecordingAndExecute called without BeginRecording.\n");
		return;
	}

	// コマンドリストを閉じる
	HRESULT result = commandList_->Close();
	assert(SUCCEEDED(result));

	// コマンドキューに実行を指示
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);

	isRecording_ = false;
}

void Cygnus::CommandManager::WaitForGPU()
{
	// フェンスにシグナルを送信
	SignalFence();

	// 最新のフェンス値の完了を待機
	WaitForFenceValue(fenceValue_);
}

void Cygnus::CommandManager::PrepareNextFrame()
{
	HRESULT result = S_FALSE;

	// フェンスにシグナルを送信
	SignalFence();

	// 前フレームの完了を待機
	WaitForFenceValue(fenceValue_);

	// コマンドアロケーターをリセット
	result = commandAllocator_->Reset();
	assert(SUCCEEDED(result));

	// コマンドリストをリセット
	result = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));
}

void Cygnus::CommandManager::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;

	HRESULT result = device_->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(result));

	Log("Created CommandQueue.\n");
}

void Cygnus::CommandManager::CreateCommandAllocator()
{
	HRESULT result = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator_)
	);
	assert(SUCCEEDED(result));

	Log("Created CommandAllocator.\n");
}

void Cygnus::CommandManager::CreateCommandList()
{
	HRESULT result = device_->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator_.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList_)
	);
	assert(SUCCEEDED(result));

	Log("Created CommandList.\n");
}

void Cygnus::CommandManager::CreateSyncObjects()
{
	// フェンス生成
	HRESULT result = device_->CreateFence(
		fenceValue_, 
		D3D12_FENCE_FLAG_NONE, 
		IID_PPV_ARGS(&fence_)
	);
	assert(SUCCEEDED(result));

	// フェンスイベント生成
	fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	assert(fenceEvent_ != nullptr);

	Log("Created Fence and Event.\n");
}

void Cygnus::CommandManager::SignalFence()
{
	// フェンスの値をインクリメント
	fenceValue_++;

	// GPUにシグナルを送信
	HRESULT result = commandQueue_->Signal(fence_.Get(), fenceValue_);
	assert(SUCCEEDED(result));
}

void Cygnus::CommandManager::WaitForFenceValue(uint64_t fenceValue)
{
	// 既に完了している場合は待機不要
	if(fence_->GetCompletedValue() >= fenceValue){
		return;
	}

	// 指定したフェンス値の完了時にイベントをシグナル状態にする
	HRESULT result = fence_->SetEventOnCompletion(fenceValue, fenceEvent_);
	assert(SUCCEEDED(result));

	// イベントの完了を待機
	WaitForSingleObject(fenceEvent_, INFINITE);
}
