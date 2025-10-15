#pragma once

// C++
#include <map>
#include <string>

// Engine
#include <MyMath.h>

/// <summary>
/// アニメーションの読み込み・補間を行うクラス
/// </summary>
class AnimationLoader {
public:
	/// <summary>
	/// 位置（Float3）を表すキーフレーム構造
	/// </summary>
	struct KeyframeFloat3 {
		Float3 value; // キーフレームの値
		float time;   // キーフレームの時刻
	};

	/// <summary>
	/// 回転（Quaternion）を表すキーフレーム構造
	/// </summary>
	struct KeyframeQuaternion {
		Quaternion value; // キーフレームの値
		float time;       // キーフレームの時刻
	};

	/// <summary>
	/// 1つのノード（ボーン）に対するアニメーションデータ
	/// </summary>
	struct NodeAnimation {
		std::vector<KeyframeFloat3> translate;
		std::vector<KeyframeQuaternion> rotate;
		std::vector<KeyframeFloat3> scale;
	};

	/// <summary>
	/// アニメーション全体のデータ構造
	/// </summary>
	struct Animation {
		float duration; // アニメーション全体の尺（単位は秒）
		// NodeAnimationの集合。Node名でひけるようにしておく
		std::map<std::string, NodeAnimation> nodeAnimations;
	};

	/// <summary>
	/// Animation読み込み
	/// </summary>
	static Animation LoadAnimation(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// 任意の時刻の値を取得する : Float3
	/// </summary>
	static Float3 CalculateValue(const std::vector<KeyframeFloat3>& keyframes, float time);

	/// <summary>
	/// 任意の時刻の値を取得する : Quaternion
	/// </summary>
	static Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);
};
