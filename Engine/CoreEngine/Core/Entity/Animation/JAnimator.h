#pragma once
#include "Core/Entity/Component/JActorComponent.h"

class JSkeletalMesh;

DECLARE_DYNAMIC_DELEGATE(FOnAnimStart);

DECLARE_DYNAMIC_DELEGATE(FOnAnimFinished);

DECLARE_DYNAMIC_DELEGATE(FOnAnimBlendOut);

class JAnimator : public JActorComponent
{
public:
	JAnimator(AActor* InOwnerActor);
	~JAnimator() override = default;

public:
	uint32_t GetType() const override;

public:
	bool Serialize_Implement(std::ofstream& FileStream) override;
	bool DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Initialize() override;
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;


	
protected:
	// Target Skeleton
	JSkeletalMesh* mSkeletalMesh;

	// Interpolation Bone Info
	JArray<FMatrix> mBoneTransforms;

	// State Machine
	JHash<JText, UPtr<class JAnimationClip>> mStateMachine;

	JText           mCurrentState;
	JAnimationClip* mCurrentAnimation;

};
