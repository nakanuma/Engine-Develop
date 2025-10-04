#pragma once

// Engine
#include <Engine/3D/Camera.h>
#include <Engine/3D/InstancedObject.h>
#include <Engine/ParticleEffect/IParticleEffect.h>

/// <summary>
/// ブレンドモード
/// </summary>
enum BlendMode { 
	None, 
	Normal, 
	Add, 
	Subtract, 
	Multiply, 
	Screen,
	Alpha
};

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
	void Emit(const Float3& pos, const Float3& velocity, const float& angle) override {
		if (particles_.size() >= kMaxParticles)
			return;
		particles_.emplace_back(CreateParticle(pos, velocity, angle));
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
		// パーティクルが無ければ早期リターン
		if (particles_.empty()) return;

		auto* dx = DirectXBase::GetInstance();
		// ブレンドモードに応じてPSOを変更
		if (blendMode_ == BlendMode::None) 
		{
			dx->GetCommandList()->SetPipelineState(dx->GetPipelineStateInstancedObjectNone());
		} 
		else if (blendMode_ == BlendMode::Normal) 
		{
			dx->GetCommandList()->SetPipelineState(dx->GetPipelineStateInstancedObjectNormal());
		} 
		else if (blendMode_ == BlendMode::Add) 
		{
			dx->GetCommandList()->SetPipelineState(dx->GetPipelineStateInstancedObjectAdd());
		} 
		else if (blendMode_ == BlendMode::Subtract) 
		{
			dx->GetCommandList()->SetPipelineState(dx->GetPipelineStateInstancedObjectSubtract());
		} 
		else if (blendMode_ == BlendMode::Multiply) 
		{
			dx->GetCommandList()->SetPipelineState(dx->GetPipelineStateInstancedObjectMultiply());
		} 
		else if (blendMode_ == BlendMode::Screen) 
		{
			dx->GetCommandList()->SetPipelineState(dx->GetPipelineStateInstancedObjectScreen());
		}
		else if (blendMode_ == BlendMode::Alpha) 
		{
			dx->GetCommandList()->SetPipelineState(dx->GetPipelineStateInstancedObjectAlpha());
		}

		// オブジェクト描画
		object_.InstancedDraw();

		dx->GetCommandList()->SetPipelineState(dx->GetPipelineState());
	}

	/// <summary>
	/// クリア処理
	/// </summary>
	void Clear() override {
		particles_.clear();
		object_.UpdateMatrix();
	}

protected:
	/// <summary>
	/// パーティクル固有の生成処理
	/// </summary>
	virtual ParticleType CreateParticle(const Float3& pos, const Float3& velicity, const float& angle) = 0;

	/// <summary>
	/// パーティクル固有の更新処理
	/// </summary>
	virtual void UpdateParticle(ParticleType& particle, float dt) = 0;

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
		if (numParticles == 0) return;

		for (size_t i = 0; i < numParticles; ++i) {
			const auto& p = particles_[i];

			Matrix sclMat = Matrix::Scaling({-p.transform.scale.x, p.transform.scale.y, p.transform.scale.z});
			Matrix rotMat = Matrix::Rotation(p.transform.rotate);
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
			object_.gTransformationMatrices.data_[i].WorldInverseTranspose = Matrix::Transpose(Matrix::Inverse(world));
			object_.gTransformationMatrices.data_[i].color = p.color;
		}

		// 前フレームとのパーティクル数の差分を取って必要な無効化だけを行う
		static size_t prevNumParticles = 0;
		if(numParticles < prevNumParticles)
		// 不要分を無効化
		for (size_t i = numParticles; i < prevNumParticles; ++i) {
			object_.gTransformationMatrices.data_[i].WVP = Matrix();
			object_.gTransformationMatrices.data_[i].World = Matrix();
			object_.gTransformationMatrices.data_[i].WorldInverseTranspose = Matrix();
			object_.gTransformationMatrices.data_[i].color = {0.0f, 0.0f, 0.0f, 0.0f};
		}
		prevNumParticles = numParticles;

		object_.UpdateMatrix();
	}

protected:
	// パーティクル最大数（共通）
	static constexpr uint32_t kMaxParticles = 1024;
	// オブジェクトデータ
	InstancedObject object_;
	// 各回転軸のビルボード適用フラグ
	std::array<bool, 3> isBillboard_ = {false, false, false};
	// ブレンドモード
	BlendMode blendMode_ = BlendMode::None;

protected:
	// パーティクルのコンテナ
	std::vector<ParticleType> particles_;
	// ビルボード行列
	Matrix billboardMatrix_;
};
