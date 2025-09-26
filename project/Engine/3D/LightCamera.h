#pragma once

#include <MyMath.h>
#include <DirectXBase.h>
#include <ConstBuffer.h>

/// <summary>
/// シャドウマップ用平行光源視点カメラ
/// </summary>
class LightCamera
{
public:
	struct BoundingBox {
		Float3 min;
		Float3 max;

		Float3 Center() const { return (min + max) * 0.5f; }
		Float3 Extents() const { return (max - min) * 0.5f; }

		void GetCorners(Float3 out[8]) const {
			Float3 e = Extents();
			Float3 c = Center();

			out[0] = { c.x - e.x, c.y - e.y, c.z - e.z };
			out[1] = { c.x - e.x, c.y - e.y, c.z + e.z };
			out[2] = { c.x - e.x, c.y + e.y, c.z - e.z };
			out[3] = { c.x - e.x, c.y + e.y, c.z + e.z };
			out[4] = { c.x + e.x, c.y - e.y, c.z - e.z };
			out[5] = { c.x + e.x, c.y - e.y, c.z + e.z };
			out[6] = { c.x + e.x, c.y + e.y, c.z - e.z };
			out[7] = { c.x + e.x, c.y + e.y, c.z + e.z };
		}
	};

	struct LightCameraCB {
		Matrix lightViewProj;
	};

public:
	/// <summary>
	/// インスタンス取得
	/// </summary>
	static LightCamera* GetInstance();

	/// <summary>
	/// 平行光源の向きをセット
	/// </summary>
	void SetDirectionalLight(const Float3& dir);

	/// <summary>
	/// 行列の更新
	/// </summary>
	void UpdateViewProjection(const BoundingBox& sceneBB);

	/// <summary>
	/// ビュープロジェクション行列の取得
	/// </summary>
	const Matrix& GetViewProj() const { return viewProj_; }

	/// <summary>
	/// 定数バッファの転送
	/// </summary>
	void TransferConstantBuffer();

private:
	Float3 lightDir_;
	Matrix view_;
	Matrix proj_;
	Matrix viewProj_;

	ConstBuffer<LightCameraCB> cb_;
};

