#pragma once
#include "Float3.h"
#include "Float4.h"
#include "ConstBuffer.h"

class LightManager
{
public:
	static LightManager* GetInstance();

	void Initialize();
	// 各ライトの定数バッファをセット
	void TransferContantBuffer();

	struct DirectionalLight {
		Float4 color; // ライトの色
		Float3 direction; // ライトの向き
		float intensity; // 輝度
	};

	struct PointLight {
		Float4 color; // ライトの色
		Float3 position; // ライトの位置
		float intensity; // 輝度
		float radius; // ライトの届く最大距離
		float decay; // 減衰率
		float padding[2];
	};

	struct SpotLight {
		Float4 color;     // ライトの色
		Float3 position;  // ライトの位置
		float intensity;  // 輝度
		Float3 direction; // スポットライトの方向
		float distance;   // ライトの届く最大距離
		float decay;      // 減衰率
		float cosAngle;   // スポットライトの余弦
		float cosFalloffStart; // Falloff開始の角度
		uint32_t isActive;
	};

	static const int kMaxLight = 64;

	struct SpotLights {
		SpotLight spotLights[kMaxLight];
	};

public:
	// 平行光源の定数バッファ
	ConstBuffer<DirectionalLight> directionalLightCB_;
	// ポイントライトの定数バッファ
	ConstBuffer<PointLight> pointLightCB_;
	// スポットライトの定数バッファ
	ConstBuffer<SpotLights> spotLightsCB_;
};

