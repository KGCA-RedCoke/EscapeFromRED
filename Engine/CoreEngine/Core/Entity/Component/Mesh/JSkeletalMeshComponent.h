#pragma once
#include "Core/Entity/Component/Scene/JSceneComponent.h"

class JMaterialInstance;
class JSkeletalMesh;
class JAnimationClip;
class JAnimator;
class JSkeletalMeshObject;

class JSkeletalMeshComponent : public JSceneComponent
{
public:
	JSkeletalMeshComponent();
	JSkeletalMeshComponent(JTextView        InName,
						   AActor*          InOwnerActor      = nullptr,
						   JSceneComponent* InParentComponent = nullptr);
	~JSkeletalMeshComponent() override = default;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;

	void Draw() override;
	void DrawID(uint32_t ID) override;

public:
	Ptr<JSkeletalMesh> GetSkeletalMesh() const;
	void               SetSkeletalMesh(JTextView InSkeletalMeshPath);
	void               SetMaterialInstance(JMaterialInstance* InMaterialInstance, uint32_t InIndex = 0) const;
	void               SetAnimation(JAnimationClip* InAnimationClip) const;
	void               SetAnimator(JAnimator* InAnimator) { mAnimator = InAnimator; }

public:
	void           ShowEditor() override;
	const FMatrix& GetAnimBoneMatrix(const JText& Text) const;

private:
	UPtr<JSkeletalMeshObject> mSkeletalMeshObject = nullptr;
	JAnimator*                mAnimator           = nullptr;
};

REGISTER_CLASS_TYPE(JSkeletalMeshComponent);
