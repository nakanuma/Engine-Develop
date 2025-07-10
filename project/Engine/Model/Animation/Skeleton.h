#pragma once

// C++
#include <string>
#include <optional>
#include <map>

// Engine
#include <MyMath.h>
#include <ModelManager.h>
#include <Model/Animation/AnimationLoader.h>

/// <summary>
/// ジョイント階層と空間変換の管理
/// </summary>
class Skeleton 
{
public:
	struct Joint {
		QuaternionTransform transform; // Transform情報
		Matrix localMatrix;            // localMatrix
		Matrix skeletonSpaceMatrix;    // sletetonSpaceでの変換行列
		std::string name;              // 名前
		std::vector<int32_t> children; // 子JointのIndexのリスト。いなければ空
		int32_t index;                 // 自身のIndex
		std::optional<int32_t> parent; // 親JointのIndex。いなければnull
	};

	/// <summary>
	/// ModelDataのNodeの階層構造からSkeletonを作成
	/// </summary>
	void CreateSkeleton(const ModelManager::Node& rootNode);
	
	/// <summary>
	/// NodeからJointを作成
	/// </summary>
	int32_t CreateJoint(const ModelManager::Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	/// <summary>
	/// Skeletonの更新
	/// </summary>
	void Update();
	
	/// <summary>
	/// Skeletonに対してAnimationの適用
	/// </summary>
	void ApplyAnimation(const AnimationLoader::Animation& animation, float animationTime);



	int32_t root_;                            // RootJointのIndex
	std::map<std::string, int32_t> jointMap_; // Joint名とIndexとの辞書
	std::vector<Joint> joints_;               // 所属しているジョイント
};
