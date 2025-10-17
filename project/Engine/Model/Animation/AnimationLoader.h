#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <map>
#include <string>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <MyMath.h>

// =========================================================
// アニメーションの読み込み・補間を行うクラス
// =========================================================
class AnimationLoader {
public:
	/// <summary>
	/// 位置（Float3）を表すキーフレーム構造
	/// </summary>
	struct KeyframeFloat3 {
		Float3 value;												/* キーフレームの値 */
		float time;													/* キーフレームの時刻 */
	};

	/// <summary>
	/// 回転（Quaternion）を表すキーフレーム構造
	/// </summary>
	struct KeyframeQuaternion {
		Quaternion value;											/* キーフレームの値 */
		float time;													/* キーフレームの時刻 */
	};

	/// <summary>
	/// 1つのノード（ボーン）に対するアニメーションデータ
	/// </summary>
	struct NodeAnimation {
		std::vector<KeyframeFloat3> translate;						/* 位置 */
		std::vector<KeyframeQuaternion> rotate;						/* 回転 */
		std::vector<KeyframeFloat3> scale;							/* スケール */
	};

	/// <summary>
	/// アニメーション全体のデータ構造
	/// </summary>
	struct Animation {
		float duration;												/* アニメーション全体の尺（単位は秒） */
		std::map<std::string, NodeAnimation> nodeAnimations;		/* NodeAnimationの集合。Node名でひけるようにしておく */
	};

	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// アニメーションの読み込みを行います。
	/// </summary>
	/// <param name="directoryPath">ディレクトリパス</param>
	/// <param name="filename">ファイル名</param>
	/// <returns>アニメーションデータ</returns>
	static Animation LoadAnimation(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// 任意の時刻の値を取得します。
	/// </summary>
	/// <param name="keyframes">キーフレーム</param>
	/// <param name="time">時刻</param>
	/// <returns>任意の時刻の値（Float3）</returns>
	static Float3 CalculateValue(const std::vector<KeyframeFloat3>& keyframes, float time);

	/// <summary>
	/// 任意の時刻の値を取得します。
	/// </summary>
	/// <param name="keyframes">キーフレーム</param>
	/// <param name="time">時刻</param>
	/// <returns>任意の時刻の値（Quaternion）</returns>
	static Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);
};
