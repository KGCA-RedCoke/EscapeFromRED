#pragma once
#include "JMeshObject.h"

class JSkeletalMeshObject : public JMeshObject
{
public:
	JSkeletalMeshObject() = default;
	JSkeletalMeshObject(const JText& InName, const JArray<Ptr<JMeshData>>& InData = {});
	JSkeletalMeshObject(const JSkeletalMeshObject& Other);
	~JSkeletalMeshObject() override;

public:
	UPtr<IManagedInterface>      Clone() const override;
	[[nodiscard]] const FMatrix& GetAnimBoneMatrix(const JText& Text) const;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Tick(float DeltaTime) override;

public:
	void AddInstance(float InCameraDistance) override;
	void Draw() override;
	void DrawID(uint32_t ID) override;

public:
	void UpdateInstance_Anim(const FSkeletalMeshInstanceData& InData);

public:
	void SetAnimation(JAnimationClip* InAnimation);

	JSkeletalMesh* GetSkeletalMesh() const { return mSkeletalMesh; }

protected:
	// -------------------------- Skin Mesh Data --------------------------
	JSkeletalMesh* mSkeletalMesh;

	// -------------------------- Animation Data --------------------------
	JAnimationClip* mCurrentAnimation;
	JAnimationClip* mPreviewAnimationClip;

	bool  bTransitAnimation      = false;
	float mTransitionElapsedTime = 0.0f;
	float mTransitionDuration    = 1.0f;

	friend class GUI_Editor_SkeletalMesh;
	friend class MMeshManager;
};
