#include "Skeleton.h"

void Skeleton::CreateSkeleton(const ModelManager::Node& rootNode) {
	root_ = CreateJoint(rootNode, {}, joints_);

	// 名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : joints_) {
		jointMap_.emplace(joint.name, joint.index);
	}

	// スケルトン作成時に更新しておく
	Update();
}

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

void Skeleton::ApplyBlendedAnimation(const AnimationLoader::Animation& a, const AnimationLoader::Animation& b, float timeA, float timeB, float blendRate) {
	for (auto& joint : joints_) {
		const std::string& name = joint.name;

		// 初期状態をベースとして保存しておく
		QuaternionTransform poseA = joint.transform;
		QuaternionTransform poseB = joint.transform;

		// アニメーションAのジョイント情報を取得
		if (auto it = a.nodeAnimations.find(name); it != a.nodeAnimations.end()) {
			// timeAにおけるトランスフォームを取得
			poseA.translate = AnimationLoader::CalculateValue(it->second.translate, timeA);
			poseA.rotate = AnimationLoader::CalculateValue(it->second.rotate, timeA);
			poseA.scale = AnimationLoader::CalculateValue(it->second.scale, timeA);
		}
		// アニメーションBでのジョイント情報を取得
		if (auto it = b.nodeAnimations.find(name); it != b.nodeAnimations.end()) {
			// timeBにおけるトランスフォームを取得
			poseB.translate = AnimationLoader::CalculateValue(it->second.translate, timeB);
			poseB.rotate = AnimationLoader::CalculateValue(it->second.rotate, timeB);
			poseB.scale = AnimationLoader::CalculateValue(it->second.scale, timeB);
		}

		// 2つのアニメーションを補間して適用
		// BlendRate : 0.0f ~ 1.0f（0.0f -> アニメーションAの姿勢、0.5f -> 中間の姿勢、1.0f -> アニメーションBの姿勢 に補間）
		joint.transform.translate = Float3::Lerp(poseA.translate, poseB.translate, blendRate);
		joint.transform.rotate = Quaternion::Slerp(poseA.rotate, poseB.rotate, blendRate);
		joint.transform.scale = Float3::Lerp(poseA.scale, poseB.scale, blendRate);
	}
}
