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

	//���ļ��м���
	bool Load(const std::string& fileName);

	
	size_t GetNumBones() const { return mBones.size(); }
	const Bone& GetBone(size_t idx) const { return mBones[idx]; }
	const std::vector<Bone>& GetBones() const { return mBones; }
	const std::vector<Matrix4>& GetGlobalInvBindPoses() const { return mGlobalInvBindPoses; }
protected:
	// ����ȫ�ֵķ�������ƾ���
	void ComputeGlobalInvBindPose();
private:
	// �����е�ÿ���ͷ
	std::vector<Bone> mBones;
	// ÿ���ͷ��ȫ�ַ�������ƾ���
	std::vector<Matrix4> mGlobalInvBindPoses;
};
