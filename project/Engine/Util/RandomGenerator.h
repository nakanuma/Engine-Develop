#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <random>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Math/MyMath.h>

// =========================================================
// ランダム数字生成クラス
// =========================================================
class RandomGenerator {
public:
	/// <summary>
	/// インスタンスを取得します。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static RandomGenerator* GetInstance();

	/// <summary>
	/// ランダムなfloat値を生成します。
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns></returns>
	float RandomValue(float min, float max);

	/// <summary>
	/// ランダムなint値を生成します。
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns></returns>
	int RandomValue(int min, int max);

	/// <summary>
	/// ランダムなFloat2値を生成します。
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns></returns>
	Float2 RandomValue(const Float2& min, const Float2& max);

	/// <summary>
	/// ランダムなFloat3値を生成します。
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns></returns>
	Float3 RandomValue(const Float3& min, const Float3& max);

	/// <summary>
	/// ランダムなbool値を生成します。
	/// </summary>
	/// <param name="trueProbability">trueを返す確率（0.0f～1.0fで設定）</param>
	/// <returns></returns>
	bool RandomValueBool(float trueProbability = 0.5f);

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	RandomGenerator();

	// =========================================================
	// Member Variables
	// =========================================================

	std::mt19937 rng_;			/* 乱数生成エンジン */
};
