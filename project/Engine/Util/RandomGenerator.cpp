#include "RandomGenerator.h"

Cygnus::RandomGenerator* Cygnus::RandomGenerator::GetInstance() {
	static RandomGenerator instance;
	return &instance;
}

float Cygnus::RandomGenerator::RandomValue(float min, float max) {
	std::uniform_real_distribution<float> dist(min, max); // 一様分布
	return dist(rng_);
}

int Cygnus::RandomGenerator::RandomValue(int min, int max) {
	std::uniform_int_distribution<int> dist(min, max); // 一様分布
	return dist(rng_);
}

Cygnus::Float2 Cygnus::RandomGenerator::RandomValue(Float2 min, Float2 max) {
	return Float2{
		RandomValue(min.x, max.x), 
		RandomValue(min.y, max.y)
	}; 
}

Cygnus::Float3 Cygnus::RandomGenerator::RandomValue(const Float3& min, const Float3& max) {
	return Float3{
		RandomValue(min.x, max.x), 
		RandomValue(min.y, max.y), 
		RandomValue(min.z, max.z)
	}; 
}

bool Cygnus::RandomGenerator::RandomValueBool(float trueProbability) {
	return RandomValue(kRandomValueMin, kRandomValueMax) < trueProbability;
}

Cygnus::RandomGenerator::RandomGenerator() : rng_(std::random_device{}()) {}
