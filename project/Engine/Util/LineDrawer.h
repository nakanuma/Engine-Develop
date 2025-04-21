#pragma once

// Engine
#include <DirectXBase.h>
#include <MyMath.h>

class LineDrawer
{
public:
	static LineDrawer* GetInstance();
	void Initialize();
	void DrawLine(const Float3& start, const Float3& end);

private:
	struct Vertex {
		Float3 position;
	};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
};