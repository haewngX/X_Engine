#pragma once
#include "BoneTransform.h"
#include <string>
#include <vector>

class Skeleton
{
public:
	
	struct Bone
	{
		BoneTransform mLocalBindPose;
		std::string mName;
		int mParent;
	};

	//从文件中加载
	bool Load(const std::string& fileName);

	
	size_t GetNumBones() const { return mBones.size(); }
	const Bone& GetBone(size_t idx) const { return mBones[idx]; }
	const std::vector<Bone>& GetBones() const { return mBones; }
	const std::vector<Matrix4>& GetGlobalInvBindPoses() const { return mGlobalInvBindPoses; }
protected:
	// 计算全局的反向绑定姿势矩阵
	void ComputeGlobalInvBindPose();
private:
	// 骨骼中的每块骨头
	std::vector<Bone> mBones;
	// 每块骨头的全局反向绑定姿势矩阵
	std::vector<Matrix4> mGlobalInvBindPoses;
};
