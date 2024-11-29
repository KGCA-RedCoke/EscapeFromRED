#pragma once
#include "JMeshObject.h"

class JSkeletalMeshObject : public JMeshObject
{
public:
	JSkeletalMeshObject(const JText& InName, const JArray<Ptr<JMeshData>>& InData = {});
	JSkeletalMeshObject(const JWText& InName, const std::vector<Ptr<JMeshData>>& InData = {});
	JSkeletalMeshObject(const JSkeletalMeshObject& Other);
	~JSkeletalMeshObject() override = default;

public:
	UPtr<IManagedInterface> Clone() const override;

protected:
	void CreateBuffers(ID3D11Device* InDevice) override;
	void UpdateBoneBuffer(ID3D11DeviceContext* InDeviceContext);

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Tick(float DeltaTime) override;

public:
	void AddInstance() override;
	void Draw() override;
	void DrawID(uint32_t ID) override;
	void DrawBone();

public:
	void SetAnimation(const Ptr<JAnimationClip>& InAnimation);

protected:
	// -------------------------- Skin Mesh Data --------------------------
	Ptr<JSkeletalMesh>  mSkeletalMesh;
	Buffer::FBufferBone mInstanceBuffer_Bone;

	// -------------------------- Animation Data --------------------------
	// Remove this
	float               mElapsedTime = 0.0f;
	Ptr<JAnimationClip> mSampleAnimation;


	friend class GUI_Editor_SkeletalMesh;
	friend class MMeshManager;
};
