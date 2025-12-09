#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Logger.h>
#include <StringUtil.h>

namespace Cygnus {
/// <summary>
/// バッファリソースを作成します。
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="sizeInBytes">サイズ（バイト単位）</param>
/// <returns>バッファリソース</returns>
Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

/// <summary>
/// DepthStencilTextureを作成します。
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
/// <param name="isReading">読み取り専用フラグ</param>
/// <returns>DepthStencilTextureリソース</returns>
Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height, bool isReading);

/// <summary>
/// リソースステートの遷移を行います。
/// </summary>
/// <param name="cmdList">コマンドリスト</param>
/// <param name="resource">リソース</param>
/// <param name="beforeState">遷移前のステート</param>
/// <param name="afterState">遷移後のステート</param>
void TransitionResource(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);
}