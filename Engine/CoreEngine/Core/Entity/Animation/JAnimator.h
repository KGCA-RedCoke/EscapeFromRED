#pragma once
#include "Core/Entity/Component/JActorComponent.h"
#include "Core/Graphics/ShaderStructs.h"

class JPawnMovementComponent;
class JSkeletalMeshComponent;
class JSkeletalMesh;

class JAnimator : public JObject
{
public:
	JAnimator();
	JAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp = nullptr);
	JAnimator(const JAnimator& InCopy);
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
	void AddAnimationClip(const JText& InState, const class JAnimationClip* InClip);
	void AddAnimationClip(const JText& InState, const JText& InClipPath, bool bEnableLoop = false);
	void AddAnimLink(const JText& SrcState, const JText& DstState, const std::function<bool()>& InFunc,
					 const float  InTransitionTime);
	void SetState(const JText& InState);

public:
	FSkeletalMeshInstanceData GetAnimInstanceData() const { return mSkeletalMeshInstanceData; }

protected:
	/**
	 * @return 애니메이션 경과 비율 반환
	 */
	float GetAnimElapsedRatio() const;

protected:
	JSkeletalMeshComponent* mSkeletalMeshComponent;

	FSkeletalMeshInstanceData mSkeletalMeshInstanceData;

	// State Machine
	JHash<JText, UPtr<class JAnimationClip>> mStateMachine;
	JHash<JText, JArray<JText>>              mAnimLink;
	JText                                    mCurrentState;
	JAnimationClip*                          mCurrentAnimation;

	// Model Data
	JPawnMovementComponent* mMovementComponent;


	friend class GUI_Editor_Animator;
};
