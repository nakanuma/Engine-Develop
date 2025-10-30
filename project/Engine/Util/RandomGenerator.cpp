#include "RandomGenerator.h"

RandomGenerator* RandomGenerator::GetInstance() {
	static RandomGenerator instance;
	return &instance;
}

float RandomGenerator::RandomValue(float min, float max) {
	std::uniform_real_distribution<float> dist(min, max); // 一様分布
	return dist(rng_);
}

int RandomGenerator::RandomValue(int min, int max) {
	std::uniform_int_distribution<int> dist(min, max); // 一様分布
	return dist(rng_);
}

Float2 RandomGenerator::RandomValue(const Float2& min, const Float2& max) { 
	return Float2{
		RandomValue(min.x, max.x), 
		RandomValue(min.y, max.y)
	}; 
}

Float3 RandomGenerator::RandomValue(const Float3& min, const Float3& max) { 
	return Float3{
		RandomValue(min.x, max.x), 
		RandomValue(min.y, max.y), 
		RandomValue(min.z, max.z)
	}; 
}

bool RandomGenerator::RandomValueBool(float trueProbability) { 
	return RandomValue(0.0f, 1.0f) < trueProbability; 
}

RandomGenerator::RandomGenerator() : rng_(std::random_device{}()) {}
