#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <ConstBuffer.h>
#include <DirectXBase.h>
#include <MyMath.h>

// =========================================================
// 平行光源視点のカメラ管理クラス
// シャドウマップ生成に使用
// =========================================================
class LightCamera {
public:
	/// <summary>
	/// AABB構造体
	/// </summary>
	struct BoundingBox {
		Float3 min;		/* 最小座標 */
		Float3 max;		/* 最大座標 */

		/// <summary>
		/// 中心座標を取得
		/// </summary>
		/// <returns>中心座標</returns>
		Float3 Center() const { return (min + max) * 0.5f; }

		/// <summary>
		/// 範囲を取得
		/// </summary>
		/// <returns>範囲</returns>
		Float3 Extents() const { return (max - min) * 0.5f; }

		/// <summary>
		/// 中心座標と範囲を設定
		/// </summary>
		/// <param name="center">中心座標</param>
		/// <param name="extents">範囲</param>
		void SetCenterExtents(const Float3& center, const Float3& extents) {
			min = center - extents;
			max = center + extents;
		}

		/// <summary>
		/// コーナー座標を取得
		/// </summary>
		/// <param name="out">コーナー座標を格納する配列</param>
		void GetCorners(Float3 out[8]) const {
			Float3 e = Extents();
			Float3 c = Center();

			out[0] = {c.x - e.x, c.y - e.y, c.z - e.z};
			out[1] = {c.x - e.x, c.y - e.y, c.z + e.z};
			out[2] = {c.x - e.x, c.y + e.y, c.z - e.z};
			out[3] = {c.x - e.x, c.y + e.y, c.z + e.z};
			out[4] = {c.x + e.x, c.y - e.y, c.z - e.z};
			out[5] = {c.x + e.x, c.y - e.y, c.z + e.z};
			out[6] = {c.x + e.x, c.y + e.y, c.z - e.z};
			out[7] = {c.x + e.x, c.y + e.y, c.z + e.z};
		}
	};

	/// <summary>
	/// ライトカメラ用の定数バッファデータ構造体
	/// </summary>
	struct LightCameraCB {
		Matrix lightViewProj;	/* ライトカメラのビュープロジェクション行列 */
	};

public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static LightCamera* GetInstance();

	/// <summary>
	/// 行列の更新を行います。
	/// </summary>
	/// <param name="sceneBB">更新を行う範囲のAABB</param>
	void UpdateViewProjection(const BoundingBox& sceneBB);

	/// <summary>
	/// 定数バッファを転送します。
	/// </summary>
	void TransferConstantBuffer();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// 平行光源の向きをセットします。
	/// </summary>
	/// <param name="dir">平行光源の向き</param>
	void SetDirectionalLight(const Float3& dir);

	/// <summary>
	/// ビュープロジェクション行列を取得します。
	/// </summary>
	const Matrix& GetViewProj() const { return viewProj_; }

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Parameters -----
	Float3 lightDir_;						/* 平行光源の向き */
	Matrix view_;							/* ライトカメラのビュー行列 */
	Matrix proj_;							/* ライトカメラのプロジェクション行列 */
	Matrix viewProj_;						/* ライトカメラのビュープロジェクション行列 */

	ConstBuffer<LightCameraCB> cb_;			/* ライトカメラ用定数バッファ */
};
