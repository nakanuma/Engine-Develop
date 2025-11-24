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
	/// <summary>
	/// 平行光源情報の構造体
	/// </summary>
	struct DirectionalLight {
		Float4 color;						/* ライトの色 */
		Float3 direction;					/* ライトの向き */
		float intensity;					/* 輝度 */
	};

	/// <summary>
	/// 点光源情報の構造体
	/// </summary>
	struct PointLight {
		Float4 color;						/* ライトの色 */
		Float3 position;					/* ライトの位置 */
		float intensity;					/* 輝度 */
		float radius;						/* ライトの届く最大距離 */
		float decay;						/* 減衰率 */
		float padding[2];					/* パディング */
	};

	/// <summary>
	/// スポットライト情報の構造体
	/// </summary>
	struct SpotLight {
		Float4 color;						/* ライトの色 */
		Float3 position;					/* ライトの位置 */
		float intensity;					/* 輝度 */
		Float3 direction;					/* スポットライトの方向 */
		float distance;						/* ライトの届く最大距離 */
		float decay;						/* 減衰率 */
		float cosAngle;						/* スポットライトの余弦 */
		float cosFalloffStart;				/* Falloff開始の角度 */
		uint32_t isActive;					/* ライト有効フラグ */
	};

	struct EmissiveLight{
		Float4 color;		/* ライトの色 */
		Float3 position;	/* ライトの位置 */
		float intensity;	/* 輝度 */
		float radius;		/* ライトの届く最大距離 */
		float decay;		/* 減衰率 */
		uint32_t isActive;	/* ライト有効フラグ */
		float padding;		/* パディング */
	};

	static constexpr uint32_t kMaxLight = 64;			/* スポットライトの最大数 */
	static constexpr uint32_t kMaxEmissiveLight = 16;	/* エミッシブライトの最大数 */

	/// <summary>
	/// 複数のスポットライトをまとめて管理する構造体
	/// </summary>
	struct SpotLights {
		SpotLight spotLights[kMaxLight];	/* スポットライト配列 */
	};

	/// <summary>
	/// 複数のエミッシブライトをまとめて管理する構造体
	/// </summary>
	struct EmissiveLights{
		EmissiveLight emissiveLights[kMaxEmissiveLight];	/* エミッシブライト配列 */
		int32_t numActiveLights;							/* 有効ライト数 */
		float padding[3];									/* パディング */
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

public:
	// ----- ConstBuffer -----
	ConstBuffer<DirectionalLight> directionalLightCB_;		/* 平行光源の定数バッファ */
	ConstBuffer<PointLight> pointLightCB_;					/* 点光源の定数バッファ */
	ConstBuffer<SpotLights> spotLightsCB_;					/* 複数のスポットライトの定数バッファ */
	ConstBuffer<EmissiveLights> emissiveLightsCB_;			/* 複数のエミッシブライトの定数バッファ */

private:
	// =========================================================
	// Constants
	// =========================================================

	// ----- RootParameterIndex -----
	static constexpr uint32_t kRootParameterIndexDirectionalLight = 3;	/* 平行光源用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexPointLight = 6;		/* 点光源用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexSpotLight = 7;			/* スポットライト用ルートパラメーターインデックス */
	static constexpr uint32_t kRootParameterIndexEmissiveLight = 14;	/* エミッシブライト用ルートパラメーターインデックス */

	// ----- Default -----
	static constexpr Float4 kDefaultLightColor = { 1.0f, 1.0f, 1.0f, 1.0f };	/* ライトのデフォルトカラー（白） */

	// ----- DirectionalLight -----
	static constexpr Float3 kDefaultDirection = { 0.0f, -1.0f, 0.0f };	/* 平行光源のライト向き（真下） */
	static constexpr float kDefaultDirectionIntensity = 1.0f;			/* 平行光源の輝度 */

	// ----- PointLight -----
	static constexpr Float3 kDefaultPointPosition = { 0.0f, 2.0f, 0.0f };	/* 点光源の位置 */
	static constexpr float kDefaultPointIntensity = 0.0f;					/* 点光源の輝度 */
	static constexpr float kDefaultPointRadius = 5.0f;						/* 点光源の半径 */
	static constexpr float kDefaultPointDecay = 1.0f;						/* 点光源の減衰率 */

	// ----- SpotLight -----
	static constexpr Float3 kDefaultSpotPosition = { 0.0f, 2.0f, 0.0f };	/* スポットライトの位置 */
	static constexpr float kDefaultSpotDistance = 50.0f;					/* スポットライトの最大距離 */
	static constexpr Float3 kDefaultSpotDirection = { 0.0f, -1.0f, 0.0f };	/* スポットライトのライト向き（真下） */
	static constexpr float kDefaultSpotIntensity = 0.0f;					/* スポットライトの輝度 */
	static constexpr float kDefaultSpotDecay = 2.0f;						/* スポットライトの減衰率 */
	static constexpr float kDefaultSpotCosAngle = 0.45f;					/* スポットライトの余弦 */
	static constexpr float kDefaultSpotCosFalloffStart = 0.55f;				/* スポットライトのFalloff開始の余弦 */

	// ----- EmissiveLight -----
	static constexpr Float3 kDefaultEmissivePosition = {0.0f, 0.0f, 0.0f};		/* エミッシブライトの位置 */
	static constexpr Float4 kDefaultEmissiveColor = {0.0f, 0.0f, 0.0f, 1.0f};	/* エミッシブライトの色 */
	static constexpr float kDefaultEmissiveIntensity = 0.0f;					/* エミッシブライトの輝度 */
	static constexpr float kDefaultEmissiveRadius = 0.0f;						/* エミッシブライトの範囲 */
	static constexpr float kDefaultEmissiveDecay = 2.0f;						/* エミッシブライトの減衰率 */

	// =========================================================
	// Member Variables
	// =========================================================
	int32_t currentEmissiveLightCount_ = 0;	/* 現在登録されているエミッシブライトの数 */
};
