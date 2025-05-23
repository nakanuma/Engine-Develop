#pragma once

// Engine
#include <Engine/3D/Camera.h>
#include <Engine/3D/InstancedObject.h>
#include <Engine/ParticleEffect/IParticleEffect.h>

/// <summary>
/// パーティクル共通の基本処理
/// </summary>
template<typename ParticleType> class BaseParticleEffect : public IParticleEffect {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseParticleEffect() = default;

	/// <summary>
	/// 生成処理
	/// </summary>
	void Emit(const Float3& pos) override {
		if (particles_.size() >= kMaxParticles)
			return;
		particles_.emplace_back(CreateParticle(pos));
	}

	/// <summary>
	/// 更新処理
	/// </summary>
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
	/// 描画処理
	/// </summary>
	void Draw() override {
		auto* dx = DirectXBase::GetInstance();
		dx->GetCommandList()->SetPipelineState(dx->GetPipelineStateInstancedObject());
		object_.InstancedDraw();
		dx->GetCommandList()->SetPipelineState(dx->GetPipelineState());
	}

protected:
	static constexpr uint32_t kMaxParticles = 1024;
	std::vector<ParticleType> particles_;
	InstancedObject object_;
	Matrix billboardMatrix_;
	std::array<bool, 3> isBillboard_ = {false, false, false};

	/// <summary>
	/// パーティクル固有の生成処理
	/// </summary>
	virtual ParticleType CreateParticle(const Float3& pos) = 0;

	/// <summary>
	/// パーティクル固有の更新処理
	/// </summary>
	virtual void UpdateParticle(ParticleType& particle, float dt) = 0;

	void UpdateInstanceMatrices() {
		Matrix view = Camera::GetCurrent()->MakeViewMatrix();
		Matrix projection = Camera::GetCurrent()->MakePerspectiveFovMatrix();
		
		// ビルボード行列を計算
		billboardMatrix_ = Matrix::Inverse(view);

		size_t numParticles = particles_.size();
		for (size_t i = 0; i < numParticles; ++i) {
			const auto& p = particles_[i];

			Matrix sclMat = Matrix::Scaling({-p.transform.scale.x, p.transform.scale.y, p.transform.scale.z});
			Matrix rotMat = Matrix::RotationRollPitchYaw(p.transform.rotate.x, p.transform.rotate.y, p.transform.rotate.z);
			Matrix tlsMat = Matrix::Translation(p.transform.translate);

			// 各軸に対してビルボード行列を使用するかチェック
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

			// ワールド行列を計算
			Matrix world = sclMat * rotMat * tlsMat;

			object_.gTransformationMatrices.data_[i].WVP = world * view * projection;
			object_.gTransformationMatrices.data_[i].World = world;
			object_.gTransformationMatrices.data_[i].WorldInverseTranspose = Matrix::Inverse(world);
			object_.gTransformationMatrices.data_[i].color = p.color;
		}

		// 残りの unused instance をクリア（透明に）
		for (size_t i = numParticles; i < kMaxParticles; ++i) {
			object_.gTransformationMatrices.data_[i].WVP = Matrix();
			object_.gTransformationMatrices.data_[i].World = Matrix();
			object_.gTransformationMatrices.data_[i].WorldInverseTranspose = Matrix();
			object_.gTransformationMatrices.data_[i].color = {0.0f, 0.0f, 0.0f, 0.0f};
		}

		object_.UpdateMatrix();
	}
};
