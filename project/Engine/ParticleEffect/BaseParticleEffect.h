#pragma once

// Engine
#include <Engine/ParticleEffect/IParticleEffect.h>
#include <Engine/3D/InstancedObject.h>
#include <Engine/3D/Camera.h>

/// <summary>
/// パーティクル共通の基本処理
/// </summary>
template<typename ParticleType>
class BaseParticleEffect : public IParticleEffect
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseParticleEffect() = default;

	/// <summary>
	/// 生成処理
	/// </summary>
	void Emit(const Float3& pos) override 
	{
		if (particles_.size() >= kMaxParticles) return;
		particles_.emplace_back(CreateParticle(pos));
	}

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(float deltaTime) override 
	{
		for (auto it = particles_.begin(); it != particles_.end();) 
		{
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
	void Draw() override 
	{ 
		auto* dx = DirectXBase::GetInstance();
		dx->GetCommandList()->SetPipelineState(dx->GetPipelineStateInstancedObject());
		object_.InstancedDraw();
		dx->GetCommandList()->SetPipelineState(dx->GetPipelineState());
	}

protected:
	static constexpr uint32_t kMaxParticles = 10;
	std::vector<ParticleType> particles_;
	InstancedObject object_;

	virtual ParticleType CreateParticle(const Float3& pos) = 0;
	virtual void UpdateParticle(ParticleType& particle, float dt) = 0;

	void UpdateInstanceMatrices() {
		Matrix view = Camera::GetCurrent()->MakeViewMatrix();
		Matrix projection = Camera::GetCurrent()->MakePerspectiveFovMatrix();

		for (size_t i = 0; i < particles_.size(); ++i) {
			const auto& p = particles_[i];

			Matrix world = Matrix::Scaling({-p.transform.scale.x, p.transform.scale.y, p.transform.scale.z}) *
			               Matrix::RotationRollPitchYaw(p.transform.rotate.x, p.transform.rotate.y, p.transform.rotate.z) * Matrix::Translation(p.transform.translate);

			object_.gTransformationMatrices.data_[i].WVP = world * view * projection;
			object_.gTransformationMatrices.data_[i].World = world;
			object_.gTransformationMatrices.data_[i].WorldInverseTranspose = Matrix::Inverse(world);
			object_.gTransformationMatrices.data_[i].color = p.color;
		}

		// 残りの unused instance をクリア（透明に）
		for (size_t i = particles_.size(); i < kMaxParticles; ++i) {
			object_.gTransformationMatrices.data_[i].WVP = Matrix();
			object_.gTransformationMatrices.data_[i].World = Matrix();
			object_.gTransformationMatrices.data_[i].WorldInverseTranspose = Matrix();
			object_.gTransformationMatrices.data_[i].color = {0.0f, 0.0f, 0.0f, 0.0f};
		}

		object_.UpdateMatrix();
	}
};
