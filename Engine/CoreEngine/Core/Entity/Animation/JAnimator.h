#pragma once
#include "Core/Entity/Component/JActorComponent.h"
#include "Core/Graphics/ShaderStructs.h"

class JSkeletalMeshComponent;
class JSkeletalMesh;

DECLARE_DYNAMIC_DELEGATE(FOnAnimStart);

DECLARE_DYNAMIC_DELEGATE(FOnAnimFinished);

DECLARE_DYNAMIC_DELEGATE(FOnAnimBlendOut);

class JAnimator : public JAsset, public IManagedInterface
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
	void Initialize();
	void BeginPlay();
	void Tick(float DeltaTime);
	void Destroy();

public:
	void AddAnimationClip(const JText& InState, class JAnimationClip* InClip);
	void AddAnimationClip(const JText& InState, const JText& InClipPath);
	void AddAnimLink(const JText& SrcState, const JText& DstState);
	void SetState(const JText& InState);

protected:
	JSkeletalMeshComponent* mSkeletalMeshComponent;

	FSkeletalMeshInstanceData mSkeletalMeshInstanceData;

	// State Machine
	JHash<JText, class JAnimationClip*> mStateMachine;
	JText                               mCurrentState;
	JAnimationClip*                     mCurrentAnimation;
	JAnimationClip*                     mPrevAnimation;

	friend class GUI_Editor_Animator;
};
