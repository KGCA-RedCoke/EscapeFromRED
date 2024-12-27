#pragma once
#include "Core/Entity/Component/JActorComponent.h"
#include "Core/Graphics/ShaderStructs.h"

class JPawnMovementComponent;
class JSkeletalMeshComponent;
class JSkeletalMesh;

DECLARE_DYNAMIC_DELEGATE(FOnAnimStart);

DECLARE_DYNAMIC_DELEGATE(FOnAnimFinished);

DECLARE_DYNAMIC_DELEGATE(FOnAnimBlendOut);

class JAnimator : public JObject
{
public:
	JAnimator();
	JAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp = nullptr);
	~JAnimator() override = default;

public:
	UPtr<IManagedInterface> Clone() const override;
	uint32_t                GetHash() const override;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Initialize() override;
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

public:
	void AddAnimationClip(const JText& InState, class JAnimationClip* InClip);
	void AddAnimationClip(const JText& InState, const JText& InClipPath);
	void AddAnimLink(const JText& SrcState, const JText& DstState, const std::function<bool()>& InFunc,
					 const float  InTransitionTime);
	void SetState(const JText& InState);

public:
	FSkeletalMeshInstanceData GetAnimInstanceData() const { return mSkeletalMeshInstanceData; }

protected:
	JSkeletalMeshComponent* mSkeletalMeshComponent;

	FSkeletalMeshInstanceData mSkeletalMeshInstanceData;

	// State Machine
	JHash<JText, class JAnimationClip*> mStateMachine;
	JHash<JText, JArray<JText>>         mAnimLink;
	JText                               mCurrentState;
	JAnimationClip*                     mCurrentAnimation;
	JAnimationClip*                     mPrevAnimation;

	// Model Data
	JPawnMovementComponent* mMovementComponent;


	friend class GUI_Editor_Animator;
};
