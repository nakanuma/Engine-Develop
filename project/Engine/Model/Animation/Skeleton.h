#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <map>
#include <optional>
#include <string>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Model/Animation/AnimationLoader.h>
#include <ModelManager.h>
#include <MyMath.h>

namespace Cygnus {
// =========================================================
// スケルトンクラス（ジョイント階層と空間変換の管理）
// =========================================================
class Skeleton {
public:
	/// <summary>
	/// スケルトンアニメーションにおける1つのジョイント（ボーン）を表す構造体
	/// </summary>
	struct Joint {
		QuaternionTransform transform;			/* Transform情報 */
		Matrix localMatrix;						/* localMatrix */
		Matrix skeletonSpaceMatrix;				/* sletetonSpaceでの変換行列 */
		std::string name;						/* 名前 */
		std::vector<int32_t> children;			/* 子JointのIndexのリスト。いなければ空 */
		int32_t index;							/* 自身のIndex */
		std::optional<int32_t> parent;			/* 親JointのIndex。いなければnull */
	};

	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// ModelDataのNodeの階層構造からSkeletonを作成します。
	/// </summary>
	/// <param name="rootNode">ルートノード</param>
	void CreateSkeleton(const ModelManager::Node& rootNode);

	/// <summary>
	/// NodeからJointを作成します。
	/// </summary>
	/// <param name="node">ノード</param>
	/// <param name="parent">親JointのIndex</param>
	/// <param name="joints">作成したJointを格納する配列</param>
	/// <returns>作成したJointのIndex</returns>
	int32_t CreateJoint(const ModelManager::Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	/// <summary>
	/// Skeletonの毎フレーム更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// Skeletonに対してAnimationの適用を行います。
	/// </summary>
	/// <param name="animation">アニメーションデータ</param>
	/// <param name="animationTime">アニメーションの時刻</param>
	void ApplyAnimation(const AnimationLoader::Animation& animation, float animationTime);

	/// <summary>
	/// アニメーションのブレンドを行います。
	/// </summary>
	/// <param name="a">アニメーションA</param>
	/// <param name="b">アニメーションB</param>
	/// <param name="timeA">アニメーションAの時刻</param>
	/// <param name="timeB">アニメーションBの時刻</param>
	/// <param name="blendRate">ブレンド率（0.0f～1.0f）</param>
	void ApplyBlendedAnimation(const AnimationLoader::Animation& a, const AnimationLoader::Animation& b, float timeA, float timeB, float blendRate);

	// =========================================================
	// Member Variables
	// =========================================================

	int32_t root_;									/* RootJointのIndex */
	std::map<std::string, int32_t> jointMap_;		/* Joint名とIndexとの辞書 */
	std::vector<Joint> joints_;						/* 所属しているジョイント */
};
}