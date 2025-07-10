#include "Skeleton.h"

// ---------------------------------------------------------
// ModelDataのNodeの階層構造からSkeletonを作成
// ---------------------------------------------------------
void Skeleton::CreateSkeleton(const ModelManager::Node& rootNode) {
	root_ = CreateJoint(rootNode, {}, joints_);

	// 名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : joints_) {
		jointMap_.emplace(joint.name, joint.index);
	}

	// スケルトン作成時に更新しておく
	Update();
}

// ---------------------------------------------------------
// NodeからJointを作成
// ---------------------------------------------------------
int32_t Skeleton::CreateJoint(const ModelManager::Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) { 
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = Matrix::Identity();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size()); // 現在登録されてる数をIndexに
	joint.parent = parent;
	joints.push_back(joint); // SkeletonのJoint列に追加
	for (const ModelManager::Node& child : node.children) {
		// 子Jointを作成し、そのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}
	// 自身のIndexを返す
	return joint.index;
}

// ---------------------------------------------------------
// Skeletonの更新
// ---------------------------------------------------------
void Skeleton::Update() {
	// すべてのJointを更新
	for (Joint& joint : joints_) {
		joint.localMatrix = joint.transform.MakeAffineMatrix();
		if (joint.parent) { // 親がいれば親の行列を掛ける
			joint.skeletonSpaceMatrix = joint.localMatrix * joints_[*joint.parent].skeletonSpaceMatrix;
		} else { // 親がいないのでlocalMatrixとskeletonSpaceMatrixは一致する
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

// ---------------------------------------------------------
// Skeletonに対してAnimationの適用
// ---------------------------------------------------------
void Skeleton::ApplyAnimation(const AnimationLoader::Animation& animation, float animationTime) {
	for (Joint& joint : joints_) {
		// 対象のJointのAnimationがあれば、他の適用を行う
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
			const AnimationLoader::NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translate = AnimationLoader::CalculateValue(rootNodeAnimation.translate, animationTime);
			joint.transform.rotate = AnimationLoader::CalculateValue(rootNodeAnimation.rotate, animationTime);
			joint.transform.scale = AnimationLoader::CalculateValue(rootNodeAnimation.scale, animationTime);
		}
	}
}
