#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <ConstBuffer.h>
#include <Float3.h>
#include <Float4.h>

// =========================================================
// 各ライトの管理クラス
// =========================================================
class LightManager {
public:
	static constexpr uint32_t kMaxLight = 64;         /* スポットライトの最大数 */
	static constexpr uint32_t kMaxEmissiveLight = 16; /* エミッシブライトの最大数 */
	static constexpr uint32_t kMaxAreaLight = 16;     /* エリアライトの最大数 */

	/// <summary>
	/// エリアライトの種類
	/// </summary>
	enum class AreaLightType {
		RectAngle, /* 矩形 */
		Disk,      /* 円盤 */
		Tube,      /* 線分 */
		Sphere     /* 球形 */
	};

public:
	/// <summary>
	/// 平行光源情報の構造体
	/// </summary>
	struct DirectionalLight {
		Float4 color;     /* ライトの色 */
		Float3 direction; /* ライトの向き */
		float intensity;  /* 輝度 */
	};

	/// <summary>
	/// 点光源情報の構造体
	/// </summary>
	struct PointLight {
		Float4 color;     /* ライトの色 */
		Float3 position;  /* ライトの位置 */
		float intensity;  /* 輝度 */
		float radius;     /* ライトの届く最大距離 */
		float decay;      /* 減衰率 */
		float padding[2]; /* パディング */
	};

	/// <summary>
	/// スポットライト情報の構造体
	/// </summary>
	struct SpotLight {
		Float4 color;          /* ライトの色 */
		Float3 position;       /* ライトの位置 */
		float intensity;       /* 輝度 */
		Float3 direction;      /* スポットライトの方向 */
		float distance;        /* ライトの届く最大距離 */
		float decay;           /* 減衰率 */
		float cosAngle;        /* スポットライトの余弦 */
		float cosFalloffStart; /* Falloff開始の角度 */
		uint32_t isActive;     /* ライト有効フラグ */
	};

	/// <summary>
	/// 複数のスポットライトをまとめて管理する構造体
	/// </summary>
	struct SpotLights {
		SpotLight spotLights[kMaxLight]; /* スポットライト配列 */
	};

	/// <summary>
	/// エミッシブライト情報の構造体
	/// </summary>
	struct EmissiveLight {
		Float4 color;      /* ライトの色 */
		Float3 position;   /* ライトの位置 */
		float intensity;   /* 輝度 */
		float radius;      /* ライトの届く最大距離 */
		float decay;       /* 減衰率 */
		uint32_t isActive; /* ライト有効フラグ */
		float padding;     /* パディング */
	};

	/// <summary>
	/// 複数のエミッシブライトをまとめて管理する構造体
	/// </summary>
	struct EmissiveLights {
		EmissiveLight emissiveLights[kMaxEmissiveLight]; /* エミッシブライト配列 */
		int32_t numActiveLights;                         /* 有効ライト数 */
		float padding[3];                                /* パディング */
	};

	/// <summary>
	/// エリアライト情報の構造体
	/// </summary>
	struct AreaLight {
		Float4 color;       /* 色 */
		Float3 position;    /* 位置 */
		float intensity;    /* 輝度 */
		Float3 right;       /* 右方向（正規化） */
		float width;        /* 幅 */
		Float3 up;          /* 上方向（正規化） */
		float height;       /* 高さ */
		Float3 normal;      /* 法線方向（正規化） */
		float range;        /* 影響範囲 */
		uint32_t isActive;  /* 有効フラグ */
		uint32_t lightType; /* タイプ */
		float padding[2];   /* パディング */
	};

	/// <summary>
	/// 複数のエリアライトをまとめて管理する構造体
	/// </summary>
	struct AreaLights {
		AreaLight areaLights[kMaxAreaLight]; /* エリアライト配列 */
		int32_t numActiveLights;             /* 有効ライト数 */
		float padding[3];                    /* パディング */
	};

public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static LightManager* GetInstance();

	/// <summary>
	/// ライト管理クラスの初期化処理を行います。
	/// </summary>
	void Initialize();

	/// <summary>
	/// 各ライトの定数バッファを転送します。
	/// </summary>
	void TransferContantBuffer();

	/// <summary>
	/// ライトのデバッグ描画を行います。
	/// </summary>
	void DrawDebug();

	/// <summary>
	/// エミッシブライトを登録します。
	/// </summary>
	/// <param name="position">位置</param>
	/// <param name="color">色</param>
	/// <param name="intensity">輝度</param>
	/// <param name="radius">範囲</param>
	/// <param name="decay">減衰率</param>
	void RegisterEmissiveLight(const Float3& position, const Float3& color, float intensity, float radius, float decay = 2.0f);

	/// <summary>
	/// エミッシブライトをクリアします。
	/// </summary>
	void ClearEmissiveLights();

	/// <summary>
	/// エリアライトを登録します。
	/// </summary>
	/// <param name="position">位置</param>
	/// <param name="right">右方向（正規化）</param>
	/// <param name="up">上方向（正規化）</param>
	/// <param name="width">幅</param>
	/// <param name="height">高さ</param>
	/// <param name="color">色</param>
	/// <param name="intensity">輝度</param>
	/// <param name="range">影響範囲</param>
	/// <param name="type">ライトタイプ</param>
	void RegisterAreaLight(
	    const Float3& position, const Float3& right, const Float3& up, float width, float height, const Float3& color, float intensity, float range, AreaLightType type = AreaLightType::RectAngle);

	/// <summary>
	/// チューブライトを登録します。
	/// </summary>
	/// <param name="start">始点</param>
	/// <param name="end">終点</param>
	/// <param name="color">色</param>
	/// <param name="intensity">輝度</param>
	/// <param name="range">影響範囲</param>
	void RegisterTubeLight(const Float3& start, const Float3& end, const Float3& color, float intensity, float range);

	/// <summary>
	/// エリアライトをクリアします。
	/// </summary>
	void ClearAreaLights();

public:
	// ----- ConstBuffer -----
	ConstBuffer<DirectionalLight> directionalLightCB_; /* 平行光源の定数バッファ */
	ConstBuffer<PointLight> pointLightCB_;             /* 点光源の定数バッファ */
	ConstBuffer<SpotLights> spotLightsCB_;             /* 複数のスポットライトの定数バッファ */
	ConstBuffer<EmissiveLights> emissiveLightsCB_;     /* 複数のエミッシブライトの定数バッファ */
	ConstBuffer<AreaLights> areaLightsCB_;             /* 複数のエリアライトの定数バッファ */

private:
	// =========================================================
	// Internal Methods
	// =========================================================
	
	/// <summary>
	/// 矩形エリアライトのデバッグ描画を行います。
	/// </summary>
	/// <param name="light">エリアライト</param>
	void DrawDebugRectangle(const LightManager::AreaLight& light);

	/// <summary>
	/// 円盤エリアライトのデバッグ描画を行います。
	/// </summary>
	/// <param name="light">エリアライト</param>
	void DrawDebugDisk(const LightManager::AreaLight& light);

	/// <summary>
	/// チューブエリアライトのデバッグ描画を行います。
	/// </summary>
	/// <param name="light">エリアライト</param>
	void DrawDebugTube(const LightManager::AreaLight& light);

	/// <summary>
	/// スフィアエリアライトのデバッグ描画を行います。
	/// </summary>
	/// <param name="light">エリアライト</param>
	/// <param name="radius">半径</param>
	void DrawDebugSphere(const LightManager::AreaLight& light);

private:
	// =========================================================
	// Constants
	// =========================================================

	// ----- Debug -----
	static constexpr Float4 kDebugDrawColor = {1.0f, 1.0f, 1.0f, 1.0f};			/* デバッグ描画用の線の色 */
	static constexpr uint32_t kDebugSubdivision = 16;							/* 球体/円盤の可視化用分割数 */

	// ----- RootParameterIndex -----
	static constexpr uint32_t kRootParameterIndexDirectionalLight = 3; /* 平行光源用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexPointLight = 6;       /* 点光源用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexSpotLight = 7;        /* スポットライト用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexEmissiveLight = 14;   /* エミッシブライト用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexAreaLight = 15;       /* エリアライト用ルートパラメーターインデックス */

	// ----- Default -----
	static constexpr Float4 kDefaultLightColor = {1.0f, 1.0f, 1.0f, 1.0f}; /* ライトのデフォルトカラー（白） */

	// ----- DirectionalLight -----
	static constexpr Float3 kDefaultDirection = {0.0f, -1.0f, 0.0f}; /* 平行光源のライト向き（真下） */
	static constexpr float kDefaultDirectionIntensity = 1.0f;        /* 平行光源の輝度 */

	// ----- PointLight -----
	static constexpr Float3 kDefaultPointPosition = {0.0f, 2.0f, 0.0f}; /* 点光源の位置 */
	static constexpr float kDefaultPointIntensity = 0.0f;               /* 点光源の輝度 */
	static constexpr float kDefaultPointRadius = 5.0f;                  /* 点光源の半径 */
	static constexpr float kDefaultPointDecay = 1.0f;                   /* 点光源の減衰率 */

	// ----- SpotLight -----
	static constexpr Float3 kDefaultSpotPosition = {0.0f, 2.0f, 0.0f};   /* スポットライトの位置 */
	static constexpr float kDefaultSpotDistance = 50.0f;                 /* スポットライトの最大距離 */
	static constexpr Float3 kDefaultSpotDirection = {0.0f, -1.0f, 0.0f}; /* スポットライトのライト向き（真下） */
	static constexpr float kDefaultSpotIntensity = 0.0f;                 /* スポットライトの輝度 */
	static constexpr float kDefaultSpotDecay = 2.0f;                     /* スポットライトの減衰率 */
	static constexpr float kDefaultSpotCosAngle = 0.45f;                 /* スポットライトの余弦 */
	static constexpr float kDefaultSpotCosFalloffStart = 0.55f;          /* スポットライトのFalloff開始の余弦 */

	// ----- EmissiveLight -----
	static constexpr Float3 kDefaultEmissivePosition = {0.0f, 0.0f, 0.0f};    /* エミッシブライトの位置 */
	static constexpr Float4 kDefaultEmissiveColor = {0.0f, 0.0f, 0.0f, 1.0f}; /* エミッシブライトの色 */
	static constexpr float kDefaultEmissiveIntensity = 0.0f;                  /* エミッシブライトの輝度 */
	static constexpr float kDefaultEmissiveRadius = 0.0f;                     /* エミッシブライトの範囲 */
	static constexpr float kDefaultEmissiveDecay = 2.0f;                      /* エミッシブライトの減衰率 */

	// ----- AreaLight -----
	static constexpr Float4 kDefaultAreaColor = {0.0f, 0.0f, 0.0f, 1.0f};                          /* エリアライトの色 */
	static constexpr Float3 kDefaultAreaPosition = {0.0f, 0.0f, 0.0f};                             /* エリアライトの位置 */
	static constexpr float kDefaultAreaIntensity = 0.0f;                                           /* エリアライトの輝度 */
	static constexpr Float3 kDefaultAreaRight = {1.0f, 0.0f, 0.0f};                                /* エリアライトの右方向 */
	static constexpr float kDefaultAreaWidth = 1.0f;                                               /* エリアライトの幅 */
	static constexpr Float3 kDefaultAreaUp = {0.0f, 1.0f, 0.0f};                                   /* エリアライトの上方向 */
	static constexpr float kDefaultAreaHeight = 1.0f;                                              /* エリアライトの高さ */
	static constexpr Float3 kDefaultAreaNormal = {0.0f, 0.0f, 1.0f};                               /* エリアライトの法線方向 */
	static constexpr float kDefaultAreaRange = 10.0f;                                              /* エリアライトの影響範囲 */
	static constexpr uint32_t kDefaultLightType = static_cast<uint32_t>(AreaLightType::RectAngle); /* エリアライトのタイプ */

	// =========================================================
	// Member Variables
	// =========================================================
	int32_t currentEmissiveLightCount_ = 0; /* 現在登録されているエミッシブライトの数 */
	int32_t currentAreaLightCount_ = 0;     /* 現在登録されているエリアライトの数 */
};
