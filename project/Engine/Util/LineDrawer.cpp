#include "LineDrawer.h"

LineDrawer* LineDrawer::GetInstance()
{
    static LineDrawer instance;
    return &instance;
}

void LineDrawer::Initialize()
{
    DirectXBase* dxBase = DirectXBase::GetInstance();

    // 頂点バッファの作成
    D3D12_HEAP_PROPERTIES heapProperties = {};
    heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC bufferDesc = {};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Width = sizeof(Vertex) * 2; // 2点のライン
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    HRESULT hr = dxBase->GetDevice()->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertexBuffer)
    );

    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = sizeof(Vertex) * 2;
    vertexBufferView.StrideInBytes = sizeof(Vertex);
}

void LineDrawer::DrawLine(const Float3& start, const Float3& end)
{
    DirectXBase* dxBase = DirectXBase::GetInstance();

    // 頂点データを準備
    Vertex vertices[] = { {start}, {end} };

    // GPUメモリへコピー
    void* mappedData = nullptr;
    D3D12_RANGE readRange = { 0, 0 };
    vertexBuffer->Map(0, &readRange, &mappedData);
    memcpy(mappedData, vertices, sizeof(vertices));
    vertexBuffer->Unmap(0, nullptr);

    // 描画設定
    dxBase->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    dxBase->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

    // 描画
    dxBase->GetCommandList()->DrawInstanced(2, 1, 0, 0);
}
