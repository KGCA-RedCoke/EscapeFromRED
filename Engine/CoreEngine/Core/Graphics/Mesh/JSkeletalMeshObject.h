#pragma once
#include "JMeshObject.h"

class JSkeletalMeshObject : public JMeshObject
{
public:
	JSkeletalMeshObject(const JText& InName, const std::vector<Ptr<JMeshData>>& InData = {});
	JSkeletalMeshObject(const JWText& InName, const std::vector<Ptr<JMeshData>>& InData = {});
	JSkeletalMeshObject(const JMeshObject& Other);
	~JSkeletalMeshObject() override = default;

public:
	Ptr<IManagedInterface> Clone() const override;

protected:
	void CreateBuffers() override;
	void UpdateBoneBuffer(ID3D11DeviceContext* InDeviceContext);

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Tick(float DeltaTime) override;

public:
	void Draw() override;
	void DrawID(uint32_t ID) override;
	void DrawBone();

private:
	// TODO: REMOVE THIS
	FMatrix InterpolateBone(int32_t InIndex, const FMatrix& InParentMatrix, float InElapsedTime);
	bool    GetAnimationTrack(float InTime, const JArray<Ptr<JAnimBoneTrack>>& InTracks, Ptr<JAnimBoneTrack>& StartTrack,
						   Ptr<JAnimBoneTrack>& EndTrack);

protected:
	// -------------------------- Skin Mesh Data --------------------------
	FSkeletonData                mSkeletonData;	// 본 계층 구조
	JHash<JText, FMatrix>        mCurrentAnimationPose;	// 재생될 현재 애니메이션의 포즈
	Ptr<class JSkinData>         mSkinData;
	Buffer::FBufferInstance_Bone mInstanceBuffer_Bone;

	// -------------------------- Animation Data --------------------------
	// Remove this
	float               mElapsedTime = 0.0f;
	Ptr<JAnimationClip> mSampleAnimation;


	friend class GUI_Editor_SkeletalMesh;
};
