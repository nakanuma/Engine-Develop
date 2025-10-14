#pragma once

// C++
#include <random>

// Engine
#include <Math/MyMath.h>

/// <summary>
/// ランダム数字生成クラス
/// </summary>
class RandomGenerator {
public:
	/// <summary>
	/// インスタンスの取得
	/// </summary>
	static RandomGenerator* GetInstance();

	/// <summary>
	/// float
	/// </summary>
	float RandomValue(float min, float max);

	/// <summary>
	/// int
	/// </summary>
	int RandomValue(int min, int max);

	/// <summary>
	/// Float2
	/// </summary>
	Float2 RandomValue(const Float2& min, const Float2& max);

	/// <summary>
	/// Float3
	/// </summary>
	Float3 RandomValue(const Float3& min, const Float3& max);

	/// <summary>
	/// bool
	/// </summary>
	/// <param name="trueProbability">trueを返す確率（0.0f ~ 1.0fで設定）</param>
	/// <returns></returns>
	bool RandomValueBool(float trueProbability = 0.5f);

private:
	RandomGenerator();
	std::mt19937 rng_;
};
