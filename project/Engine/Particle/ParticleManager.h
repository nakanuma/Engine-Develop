#pragma once
#include <random>
#include <list>
#include <unordered_map>
#include <memory>

#include "DirectXBase.h"
#include "SRVManager.h"
#include "Object3D.h"

class ParticleManager
{
public:
	struct Particle {
		Transform transform;
		Float3 velocity;
		Float4 color;
		float lifeTime;
		float currentTime;
	};

	struct ParticleGroup {
		Object3D object;
		uint32_t textureHandle;
		std::list<Particle> particles;
		StructuredBuffer<Object3D::ParticleForGPU> instancingBuffer;

		ParticleGroup() : instancingBuffer(100){}
	};

	// Field
	struct AccelerationField {
		Float3 acceleration; //!< 加速度
		AABB area; //!< 範囲
	};

public:
	ParticleManager* GetInstance();

	void Initialize(DirectXBase* dxBase, SRVManager* srvManager);
	void Update();
	void Draw();

	void CreateParticleGroup(const std::string name);
	void SetModel(const std::string name, ModelManager::ModelData* model);
	void SetTexture(const std::string name, uint32_t textureHandle);
	void Emit(const std::string name, const Float3& position, uint32_t count);

	// パーティクルグループコンテナ
	std::unordered_map<std::string, std::unique_ptr<ParticleGroup>> particleGroups;
private:
	DirectXBase* dxBase = nullptr;
	SRVManager* srvManager = nullptr;

	// 乱数生成器の初期化
	std::random_device seedGenerator;
	std::mt19937 randomEngine;

	// Δtを定義
	const float kDeltaTime = 1.0f / 60.0f;
	// 反対側に回す回転行列
	Matrix backToFrontMatrix;
	// billboard行列
	Matrix billboardMatrix;

	// Field
	AccelerationField accelerationField;
};

