#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/3D/Camera.h>
#include <Engine/3D/InstancedObject.h>
#include <Engine/ParticleEffect/IParticleEffect.h>

/// <summary>
/// ブレンドモード
/// </summary>
enum BlendMode {
	None,			/* 無効 */
	Normal,			/* 通常 */
	Add,			/* 加算 */
	Subtract,		/* 減算 */
	Multiply,		/* 乗算 */
	Screen,			/* スクリーン */
	Alpha			/* アルファ */
};

// =========================================================
// パーティクル共通の基本処理
// =========================================================
template<typename ParticleType> class BaseParticleEffect : public IParticleEffect {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseParticleEffect() = default;

	/// <summary>
	/// パーティクルの発生処理を行います。
	/// </summary>
	/// <param name="pos">位置</param>
	/// <param name="velocity">速度ベクトル</param>
	/// <param name="angle">角度</param>
	void Emit(const Float3& pos, const Float3& velocity, const float& angle) override {
		if (particles_.size() >= kMaxParticles)
			return;
		particles_.emplace_back(CreateParticle(pos, velocity, angle));
	}

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="deltaTime"></param>
	void Update(float deltaTime) override {
		for (auto it = particles_.begin(); it != particles_.end();) {
			it->currentTime += deltaTime;
			if (it->currentTime >= it->lifeTime) {
				it = particles_.erase(it);
			} else {
				UpdateParticle(*it, deltaTime);
				++it;
			}
		}

		UpdateInstanceMatrices();
	}

	/// <summary>
	/// パーティクルの描画処理を行います。
	/// </summary>
	void Draw() override {
		// パーティクルが無ければ早期リターン
		if (particles_.empty())
			return;

		auto* dx = DirectXBase::GetInstance();
		// ブレンドモードに応じてPSOを変更
		if (blendMode_ == BlendMode::None) {
			dx->GetCommandList()->SetPipelineState(dx->GetPipelineStateInstancedObjectNone());
		} else if (blendMode_ == BlendMode::Normal) {
			dx->GetCommandList()->SetPipelineState(dx->GetPipelineStateInstancedObjectNormal());
		} else if (blendMode_ == BlendMode::Add) {
			dx->GetCommandList()->SetPipelineState(dx->GetPipelineStateInstancedObjectAdd());
		} else if (blendMode_ == BlendMode::Subtract) {
			dx->GetCommandList()->SetPipelineState(dx->GetPipelineStateInstancedObjectSubtract());
		} else if (blendMode_ == BlendMode::Multiply) {
			dx->GetCommandList()->SetPipelineState(dx->GetPipelineStateInstancedObjectMultiply());
		} else if (blendMode_ == BlendMode::Screen) {
			dx->GetCommandList()->SetPipelineState(dx->GetPipelineStateInstancedObjectScreen());
		} else if (blendMode_ == BlendMode::Alpha) {
			dx->GetCommandList()->SetPipelineState(dx->GetPipelineStateInstancedObjectAlpha());
		}

		// オブジェクト描画
		object_.InstancedDraw();

		dx->GetCommandList()->SetPipelineState(dx->GetPipelineState());
	}

	/// <summary>
	/// パーティクルのクリア処理を行います。
	/// </summary>
	void Clear() override {
		particles_.clear();
		object_.UpdateMatrix();
	}

protected:
	/// <summary>
	/// パーティクル固有の生成処理を行います。
	/// </summary>
	/// <param name="pos">位置</param>
	/// <param name="velocity">速度ベクトル</param>
	/// <param name="angle">角度</param>
	/// <returns>パーティクル</returns>
	virtual ParticleType CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) = 0;

	/// <summary>
	/// パーティクル固有の毎フレーム更新処理を行います。
	/// </summary>
	/// <param name="particle">パーティクル</param>
	/// <param name="dt">デルタタイム</param>
	virtual void UpdateParticle(ParticleType& particle, float dt) = 0;

	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// インスタンス行列の更新を行います。
	/// </summary>
	void UpdateInstanceMatrices() {
		Matrix view = Camera::GetCurrent()->MakeViewMatrix();
		Matrix projection = Camera::GetCurrent()->MakePerspectiveFovMatrix();

		// ビルボード行列を計算
		billboardMatrix_ = Matrix::Inverse(view);
		billboardMatrix_.r[3][0] = 0.0f;
		billboardMatrix_.r[3][1] = 0.0f;
		billboardMatrix_.r[3][2] = 0.0f;

		size_t numParticles = particles_.size();
		// パーティクルが無ければ早期リターン
		if (numParticles == 0)
			return;

		for (size_t i = 0; i < numParticles; ++i) {
			const auto& p = particles_[i];

			Matrix sclMat = Matrix::Scaling({ -p.transform.scale.x, p.transform.scale.y, p.transform.scale.z });
			Matrix rotMat = Matrix::Rotation(p.transform.rotate);
			Matrix tlsMat = Matrix::Translation(p.transform.translate);

			// 各軸に対してビルボード行列を使用するかチェック
			bool shouldBillBoard = false;
			if (isBillboard_[0] && isBillboard_[1] && isBillboard_[2]) {
				// 3軸全てがtrueなら、完全にビルボード行列で上書き
				rotMat = billboardMatrix_;
				shouldBillBoard = true;
			} else {
				// 部分的にビルボード行列を適用
				if (isBillboard_[0]) { // X軸
					rotMat.r[0][0] = billboardMatrix_.r[0][0];
					rotMat.r[0][1] = billboardMatrix_.r[0][1];
					rotMat.r[0][2] = billboardMatrix_.r[0][2];
					rotMat.r[0][3] = billboardMatrix_.r[0][3];
				}
				if (isBillboard_[1]) {
					rotMat.r[1][0] = billboardMatrix_.r[1][0];
					rotMat.r[1][1] = billboardMatrix_.r[1][1];
					rotMat.r[1][2] = billboardMatrix_.r[1][2];
					rotMat.r[1][3] = billboardMatrix_.r[1][3];
				}
				if (isBillboard_[2]) {
					rotMat.r[2][0] = billboardMatrix_.r[2][0];
					rotMat.r[2][1] = billboardMatrix_.r[2][1];
					rotMat.r[2][2] = billboardMatrix_.r[2][2];
					rotMat.r[2][3] = billboardMatrix_.r[2][3];
				}
			}

			// 完全にビルボードが適用されている場合、Z軸周りの回転を再適用
			if(shouldBillBoard){
				Matrix zRotmat = Matrix::RotationZ(p.transform.rotate.z);
				rotMat = zRotmat * rotMat;
			}

			// ワールド行列を計算
			Matrix world = sclMat * rotMat * tlsMat;

			object_.gTransformationMatrices.data_[i].WVP = world * view * projection;
			object_.gTransformationMatrices.data_[i].World = world;
			object_.gTransformationMatrices.data_[i].WorldInverseTranspose = Matrix::Transpose(Matrix::Inverse(world));
			object_.gTransformationMatrices.data_[i].color = p.color;
		}

		// 前フレームとのパーティクル数の差分を取って必要な無効化だけを行う
		static size_t prevNumParticles = 0;
		if (numParticles < prevNumParticles)
			// 不要分を無効化
			for (size_t i = numParticles; i < prevNumParticles; ++i) {
				object_.gTransformationMatrices.data_[i].WVP = Matrix();
				object_.gTransformationMatrices.data_[i].World = Matrix();
				object_.gTransformationMatrices.data_[i].WorldInverseTranspose = Matrix();
				object_.gTransformationMatrices.data_[i].color = { 0.0f, 0.0f, 0.0f, 0.0f };
			}
		prevNumParticles = numParticles;

		object_.UpdateMatrix();
	}

protected:
	// =========================================================
	// Member Variables
	// =========================================================

	static constexpr uint32_t kMaxParticles = 1024;					/* パーティクル最大数（共通） */
	InstancedObject object_;										/* インスタンス化オブジェクト */
	std::array<bool, 3> isBillboard_ = { false, false, false };		/* 各軸ビルボードフラグ */
	BlendMode blendMode_ = BlendMode::None;							/* ブレンドモード */

	std::vector<ParticleType> particles_;							/* パーティクルコンテナ */
	Matrix billboardMatrix_;										/* ビルボード行列 */
};
