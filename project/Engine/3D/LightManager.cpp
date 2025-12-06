#include "LightManager.h"

// Engine
#include <DirectXBase.h>
#include <LineDrawer.h>

// C++
#include <numbers>

Cygnus::LightManager* Cygnus::LightManager::GetInstance() {
	static Cygnus::LightManager instance;
	return &instance;
}

void Cygnus::LightManager::Initialize() {
	// 平行光源の初期化
	directionalLightCB_.data_->color = kDefaultLightColor;
	directionalLightCB_.data_->direction = kDefaultDirection;
	directionalLightCB_.data_->intensity = kDefaultDirectionIntensity;

	// ポイントライトの初期化
	pointLightCB_.data_->color = kDefaultLightColor;
	pointLightCB_.data_->position = kDefaultPointPosition;
	pointLightCB_.data_->intensity = kDefaultPointIntensity;
	pointLightCB_.data_->radius = kDefaultPointRadius;
	pointLightCB_.data_->decay = kDefaultPointDecay;

	// スポットライトの初期化
	for (size_t i = 0; i < kMaxLight; i++) {
		spotLightsCB_.data_->spotLights[i].color = kDefaultLightColor;
		spotLightsCB_.data_->spotLights[i].position = kDefaultSpotPosition;
		spotLightsCB_.data_->spotLights[i].distance = kDefaultSpotDistance;
		spotLightsCB_.data_->spotLights[i].direction = kDefaultSpotDirection;
		spotLightsCB_.data_->spotLights[i].intensity = kDefaultSpotIntensity;
		spotLightsCB_.data_->spotLights[i].decay = kDefaultSpotDecay;
		spotLightsCB_.data_->spotLights[i].cosAngle = kDefaultSpotCosAngle;
		spotLightsCB_.data_->spotLights[i].cosFalloffStart = kDefaultSpotCosFalloffStart;
		spotLightsCB_.data_->spotLights[i].isActive = false;
	}

	// エミッシブライトの初期化
	for(size_t i = 0; i < kMaxEmissiveLight; i++){
		emissiveLightsCB_.data_->emissiveLights[i].color = kDefaultEmissiveColor;
		emissiveLightsCB_.data_->emissiveLights[i].position = kDefaultEmissivePosition;
		emissiveLightsCB_.data_->emissiveLights[i].intensity = kDefaultEmissiveIntensity;
		emissiveLightsCB_.data_->emissiveLights[i].radius = kDefaultEmissiveRadius;
		emissiveLightsCB_.data_->emissiveLights[i].decay = kDefaultEmissiveDecay;
		emissiveLightsCB_.data_->emissiveLights[i].isActive = false;
	}
	emissiveLightsCB_.data_->numActiveLights = 0;
	// カウンターを初期化
	currentEmissiveLightCount_ = 0;

	// エリアライトの初期化
	for (size_t i = 0; i < kMaxAreaLight; i++) {
		areaLightsCB_.data_->areaLights[i].color = kDefaultAreaColor;
		areaLightsCB_.data_->areaLights[i].position = kDefaultAreaPosition;
		areaLightsCB_.data_->areaLights[i].intensity = kDefaultAreaIntensity;
		areaLightsCB_.data_->areaLights[i].right = kDefaultAreaRight;
		areaLightsCB_.data_->areaLights[i].width = kDefaultAreaWidth;
		areaLightsCB_.data_->areaLights[i].up = kDefaultAreaUp;
		areaLightsCB_.data_->areaLights[i].height = kDefaultAreaHeight;
		areaLightsCB_.data_->areaLights[i].normal = kDefaultAreaNormal;
		areaLightsCB_.data_->areaLights[i].range = kDefaultAreaRange;
		areaLightsCB_.data_->areaLights[i].isActive = false;
		areaLightsCB_.data_->areaLights[i].lightType = kDefaultLightType;
	}
	areaLightsCB_.data_->numActiveLights = 0;
	// カウンターを初期化
	currentAreaLightCount_ = 0;
}

void Cygnus::LightManager::TransferContantBuffer() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 平行光源の定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(kRootParameterIndexDirectionalLight, directionalLightCB_.resource_->GetGPUVirtualAddress());
	// ポイントライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(kRootParameterIndexPointLight, pointLightCB_.resource_->GetGPUVirtualAddress());
	// スポットライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(kRootParameterIndexSpotLight, spotLightsCB_.resource_->GetGPUVirtualAddress());
	// エミッシブライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(kRootParameterIndexEmissiveLight, emissiveLightsCB_.resource_->GetGPUVirtualAddress());
	// エリアライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(kRootParameterIndexAreaLight, areaLightsCB_.resource_->GetGPUVirtualAddress());
}

void Cygnus::LightManager::DrawDebug() {
	// 有効化されているエリアライトがなければスキップ
	if (areaLightsCB_.data_->numActiveLights == 0) return;

	auto drawer = LineDrawer::GetInstance();

	for (size_t i = 0; i < areaLightsCB_.data_->numActiveLights; ++i) {
		const AreaLight& light = areaLightsCB_.data_->areaLights[i];

		// 無効化状態ならスキップ
		if (light.isActive == 0 && light.intensity == 0.0f) {
			continue;
		}

		AreaLightType type = static_cast<AreaLightType>(light.lightType);

		// 各ライトタイプに応じた発光面を描画
		switch (type) {
		case LightManager::AreaLightType::RectAngle:
			DrawDebugRectangle(light);
			break;
		case LightManager::AreaLightType::Disk:
			DrawDebugDisk(light);
			break;
		case LightManager::AreaLightType::Tube:
			DrawDebugTube(light);
			break;
		case LightManager::AreaLightType::Sphere:
			DrawDebugSphere(light);
			break;
		default:
			break;
		}
	}
}

void Cygnus::LightManager::RegisterEmissiveLight(const Float3& position, const Float3& color, float intensity, float radius, float decay)
{
	// 最大数を超えていたら登録しない
	if(currentEmissiveLightCount_ >= kMaxEmissiveLight){
		return;
	}

	// エミッシブライトを登録
	EmissiveLight& light = emissiveLightsCB_.data_->emissiveLights[currentEmissiveLightCount_];
	light.color = Float4{color.x, color.y, color.z, 1.0f};
	light.position = position;
	light.intensity = intensity;
	light.radius = radius;
	light.decay = decay;
	light.isActive = true;

	currentEmissiveLightCount_++;
	emissiveLightsCB_.data_->numActiveLights = currentEmissiveLightCount_;
}

void Cygnus::LightManager::ClearEmissiveLights()
{
	// 毎フレーム開始時に呼び出してリセット
	for(size_t i = 0; i < currentEmissiveLightCount_; i++){
		emissiveLightsCB_.data_->emissiveLights[i].isActive = 0;
	}
	currentEmissiveLightCount_ = 0;
	emissiveLightsCB_.data_->numActiveLights = 0;
}

void Cygnus::LightManager::RegisterAreaLight(const Float3& position, const Float3& right, const Float3& up, float width, float height, const Float3& color, float intensity, float range, AreaLightType type) {
	// 最大数を超えていたら登録しない
	if (currentAreaLightCount_ >= kMaxAreaLight) {
		return;
	}

	AreaLight& light = areaLightsCB_.data_->areaLights[currentAreaLightCount_];

	// 方向ベクトルを正規化
	Float3 normalizedRight = right;
	float rightLength = Float3::Length(right);
	if (rightLength > 0.0f) {
		normalizedRight.x /= rightLength;
		normalizedRight.y /= rightLength; 
		normalizedRight.z /= rightLength;
	};

	Float3 normalizedUp = up;
	float upLength = Float3::Length(up);
	if (upLength > 0.0f) {
		normalizedUp.x /= upLength;
		normalizedUp.y /= upLength;
		normalizedUp.z /= upLength;
	}

	// 法線を計算
	Float3 normal = Float3::Cross(normalizedRight, normalizedUp);

	light.color = Float4{color.x, color.y, color.z, 1.0f};
	light.position = position;
	light.intensity = intensity;
	light.right = normalizedRight;
	light.width = width;
	light.up = normalizedUp;
	light.height = height;
	light.normal = normal;
	light.range = range;
	light.isActive = true;
	light.lightType = static_cast<uint32_t>(type);

	currentAreaLightCount_++;
	areaLightsCB_.data_->numActiveLights = currentAreaLightCount_;
}

void Cygnus::LightManager::RegisterTubeLight(const Float3& start, const Float3& end, const Float3& color, float intensity, float range) {
	// チューブの方向を計算
	Float3 direction = end - start;
	float length = Float3::Length(direction);

	// 長さがほぼゼロの場合は登録しない
	if (length < 0.001f) {
		return;
	}

	// 正規化
	direction.x /= length;
	direction.y /= length;
	direction.z /= length;

	// 中心位置を計算
	Float3 center = (start + end) * 0.5f;

	// 任意の上方向を計算
	Float3 up;
	if (std::abs(direction.y) < 0.9f) {
		up = {0.0f, 1.0f, 0.0f};
	} else {
		up = {1.0f, 0.0f, 0.0f};
	}

	RegisterAreaLight(
		center, 
		direction, 
		up, 
		length, 
		0.1f, 
		color, 
		intensity, 
		range, 
		AreaLightType::Tube
	);
}

void Cygnus::LightManager::ClearAreaLights() {
	// 毎フレーム開始時に呼び出してリセット
	for (size_t i = 0; i < currentAreaLightCount_; i++) {
		areaLightsCB_.data_->areaLights[i].isActive = 0;
	}
	currentAreaLightCount_ = 0;
	areaLightsCB_.data_->numActiveLights = 0;
}

void Cygnus::LightManager::DrawDebugRectangle(const LightManager::AreaLight& light) {
	auto drawer = LineDrawer::GetInstance();

	Float3 center = light.position; 
	Float3 rightHalf = light.right * (light.width * 0.5f);
	Float3 upHalf = light.up * (light.height * 0.5f);

	// 4隅の頂点を計算
	Float3 p1 = center + rightHalf + upHalf;
	Float3 p2 = center - rightHalf + upHalf;
	Float3 p3 = center - rightHalf - upHalf;
	Float3 p4 = center + rightHalf - upHalf;

	// 4辺を描画
	drawer->RegisterLine(p1, p2, kDebugDrawColor);
	drawer->RegisterLine(p2, p3, kDebugDrawColor);
	drawer->RegisterLine(p3, p4, kDebugDrawColor);
	drawer->RegisterLine(p4, p1, kDebugDrawColor);
}

void Cygnus::LightManager::DrawDebugDisk(const LightManager::AreaLight& light) {
	auto drawer = LineDrawer::GetInstance(); 

	// Widthを半径として使用
	float radius = light.width * 0.5f;
	Float3 center = light.position;
	Float3 normal = light.normal;

	// upとrightを基底とする
	Float3 u_axis = light.right;
	Float3 v_axis = light.up;

	Float3 prevPoint;
	bool first = true;

	for (size_t i = 0; i <= kDebugSubdivision; ++i) {
		float angle = (2.0f * PIf / kDebugSubdivision) * i;

		// 円周上の点を計算
		Float3 currentPoint = center + (u_axis * std::cosf(angle) * radius) + (v_axis * std::sinf(angle) * radius);

		if (!first) {
			drawer->RegisterLine(prevPoint, currentPoint, kDebugDrawColor);
		}
		prevPoint = currentPoint;
		first = false;
	}
}

void Cygnus::LightManager::DrawDebugTube(const LightManager::AreaLight& light) {
	auto drawer = LineDrawer::GetInstance(); 

	// 半分の長さを取得
	float halfLength = light.width * 0.5f;

	// 始点と終点の計算
	Float3 start = light.position - light.right * halfLength;
	Float3 end = light.position + light.right * halfLength;

	// 線分（チューブの中心線）を描画
	drawer->RegisterLine(start, end, kDebugDrawColor);

	// 両端の円（チューブの半径）を描画
	Float3 u_axis = light.up;
	Float3 v_axis = light.normal;
	float radius = light.height;

	Float3 prevPointStart, prevPointEnd;
	bool first = true;

	for (size_t i = 0; i < kDebugSubdivision; ++i) {
		float angle = (2.0f * PIf / kDebugSubdivision) * i;

		// 円周上のオフセットを計算
		Float3 offset = (u_axis * std::cosf(angle) * radius) + (v_axis * std::sinf(angle) * radius);

		Float3 currentPointStart = start + offset;
		Float3 currentPointEnd = end + offset;

		if (!first) {
			// 両端の円を描画
			drawer->RegisterLine(prevPointStart, currentPointStart, kDebugDrawColor);
			drawer->RegisterLine(prevPointEnd, currentPointEnd, kDebugDrawColor);

			// 側線（チューブの両端を結ぶ線）を描画
			if (i % (kDebugSubdivision / 4) == 0) {
				drawer->RegisterLine(currentPointStart, currentPointEnd, kDebugDrawColor);
			}
			prevPointStart = currentPointStart;
			prevPointEnd = currentPointEnd;
			first = false;
		}
	}
}

void Cygnus::LightManager::DrawDebugSphere(const LightManager::AreaLight& light) {
	if (light.range <= 0.0f) return;

	auto drawer = LineDrawer::GetInstance(); 

	const float kLonEvery = (PIf * 2.0f) / kDebugSubdivision;
	const float kLatEvery = PIf / kDebugSubdivision;

	for (uint32_t latIndex = 0; latIndex < kDebugSubdivision; ++latIndex) {
		float lat = -PIf / 2.0f + kLatEvery * latIndex;

		for (uint32_t lonIndex = 0; lonIndex < kDebugSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;

			// 頂点a, b, cを求める
			Float3 a = {
			    cosf(lat) * cosf(lon) * light.range,
			    sinf(lat) * light.range,
			    cosf(lat) * sinf(lon) * light.range,
			};
			Float3 b = {
			    cosf(lat + kLatEvery) * cosf(lon) * light.range,
			    sinf(lat + kLatEvery) * light.range,
			    cosf(lat + kLatEvery) * sinf(lon) * light.range,
			};
			Float3 c = {
			    cosf(lat) * cosf(lon + kLonEvery) * light.range,
			    sinf(lat) * light.range,
			    cosf(lat) * sinf(lon + kLonEvery) * light.range,
			};

			// ワールド座標に移動
			a = a + light.position;
			b = b + light.position;
			c = c + light.position;

			drawer->RegisterLine(a, b, kDebugDrawColor);
			drawer->RegisterLine(a, c, kDebugDrawColor);
		}
	}
}
