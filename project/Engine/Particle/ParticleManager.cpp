#include "ParticleManager.h"
#include <cassert>
#include <numbers>

#include "Camera.h"

ParticleManager* ParticleManager::GetInstance()
{
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Initialize(DirectXBase* dxBase, SRVManager* srvManager)
{
	// 引数で受け取ったポインタをメンバ変数に記録
	this->dxBase = dxBase;
	this->srvManager = srvManager;

	// ランダムエンジンの初期化
	randomEngine.seed(seedGenerator());

	// 反対側に回す回転行列
	backToFrontMatrix = Matrix::RotationY(std::numbers::pi_v<float>);

	// 全てのパーティクルグループのObjectを初期化
	for (auto& [name, groupPtr] : particleGroups) {
		auto& group = *groupPtr;
		group.object.transform_.rotate = { 0.0f, 3.1f, 0.0f };
		// 単位行列を書き込んでおく
		for (uint32_t index = 0; index < group.instancingBuffer.numMaxInstance_; ++index) {
			group.instancingBuffer.data_[index].WVP = Matrix::Identity();
			group.instancingBuffer.data_[index].World = Matrix::Identity();
			group.instancingBuffer.data_[index].color = Float4(1.0f, 1.0f, 1.0f, 1.0f); // とりあえず白を書き込む
		}
	}

	accelerationField.acceleration = { 15.0f, 0.0f, 0.0f };
	accelerationField.area.min = { -1.0f, -1.0f, -1.0f };
	accelerationField.area.max = { 1.0f, 1.0f, 1.0f };
}

void ParticleManager::CreateParticleGroup(const std::string name)
{
	assert(particleGroups.find(name) == particleGroups.end());

	// 新たな空のパーティクルグループを作成
	auto newGroup = std::make_unique<ParticleGroup>();
	// コンテナに登録
	particleGroups[name] = std::move(newGroup);
}

void ParticleManager::SetModel(const std::string name, ModelManager::ModelData* model)
{
	auto it = particleGroups.find(name);
	assert(it != particleGroups.end()); // 登録済みのパーティクルグループかチェック

	// モデルをロードし、ParticleGroupにセット
	it->second->object.model_ = model;
}

void ParticleManager::Update()
{
	// ビュー行列とプロジェクション行列をカメラから取得
	Matrix viewMatrix = Camera::GetCurrent()->MakeViewMatrix();
	Matrix projectionMatrix = Camera::GetCurrent()->MakePerspectiveFovMatrix();

	// ビルボード行列の計算
	billboardMatrix = backToFrontMatrix * viewMatrix;
	billboardMatrix.r[3][0] = 0.0f;
	billboardMatrix.r[3][1] = 0.0f;
	billboardMatrix.r[3][2] = 0.0f;

	// 全てのパーティクルグループについて処理する
	for (auto& [name, groupPtr] : particleGroups) {
		auto& group = *groupPtr;

		group.object.UpdateMatrix();

		uint32_t numInstance = 0; // 描画すべきインスタンス数

		for (auto particleIterator = group.particles.begin(); particleIterator != group.particles.end();) {
			if (particleIterator->lifeTime <= particleIterator->currentTime) {
				particleIterator = group.particles.erase(particleIterator); // 生存期間が過ぎたParticleはlistから消す。戻り値が次のイテレーターとなる
				continue;
			}

			Matrix worldMatrix = particleIterator->transform.MakeAffineMatrix();
			Matrix viewProjectionMatrix = viewMatrix * projectionMatrix;
			Matrix worldViewProjectionMatrix = worldMatrix * billboardMatrix * viewProjectionMatrix;

			if (numInstance < group.instancingBuffer.numMaxInstance_) { // パーティクルのインスタンス数がバッファのサイズを超えないようにする
				group.instancingBuffer.data_[numInstance].WVP = worldViewProjectionMatrix;
				group.instancingBuffer.data_[numInstance].World = worldMatrix;
				group.instancingBuffer.data_[numInstance].color = particleIterator->color; // パーティクルの色をそのままコピー

				float alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime); // 経過時間に応じたAlpha値を算出
				group.instancingBuffer.data_[numInstance].color.w = alpha; // GPUに送る

				++numInstance; // 生きているParticleの数を1つカウントする
			}

			// Fieldの範囲内のParticleには加速度を適用する
			if (IsCollision(accelerationField.area, particleIterator->transform.translate)) {
				particleIterator->velocity += accelerationField.acceleration * kDeltaTime;
			}

			// Particleの更新処理
			particleIterator->transform.translate += particleIterator->velocity * kDeltaTime;
			particleIterator->currentTime += kDeltaTime; // 経過時間を足す

			++particleIterator; // 次のイテレータに進める
		}
	}
}

void ParticleManager::Draw()
{
	for (auto& [name, groupPtr] : particleGroups) {
		auto& group = *groupPtr;
		uint32_t numInstance = static_cast<uint32_t>(group.particles.size());
		group.object.DrawInstancing(group.instancingBuffer, numInstance, group.textureHandle);
	}
}

void ParticleManager::Emit(const std::string name, const Float3& position, uint32_t count)
{
	auto it = particleGroups.find(name);
	assert(it != particleGroups.end()); // 登録済みのパーティクルグループかチェック

	// 指定されたパーティクルグループを取得
	auto& group = *it->second;
	// 新たなパーティクルを作成し、指定されたパーティクルグループに登録
	for (uint32_t i = 0; i < count; ++i) {
		std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
		Particle particle;
		particle.transform.scale = { 1.0f, 1.0f, 1.0f };
		particle.transform.rotate = { 0.0f, 0.0f, 0.0f };
		particle.transform.translate = { distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };
		particle.velocity = { distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };

		Float3 randomTranslate{ distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };
		particle.transform.translate = position + randomTranslate;

		// 色をランダムに初期化
		std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
		particle.color = { distColor(randomEngine), distColor(randomEngine), distColor(randomEngine), 1.0f };

		// 生存可能時間と経過時間を初期化
		std::uniform_real_distribution<float> distTime(1.0f, 3.0f);
		particle.lifeTime = distTime(randomEngine);
		particle.currentTime = 0;

		// 新たなパーティクルをパーティクルグループに追加
		group.particles.push_back(particle);
	}
}

void ParticleManager::SetTexture(const std::string name, uint32_t textureHandle)
{
	auto it = particleGroups.find(name);
	assert(it != particleGroups.end()); // 登録済みのパーティクルグループかチェック

	// モデルをロードし、ParticleGroupにセット
	it->second->textureHandle = textureHandle;
}
