#include "JPlayerAnimator.h"

#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Interface/JWorld.h"

JPlayerAnimator::JPlayerAnimator() {}

JPlayerAnimator::JPlayerAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp)
	: JAnimator(InName, InSkeletalComp) {}

JPlayerAnimator::JPlayerAnimator(JTextView InName, AActor* InOwnerActor)
	: JAnimator(InName) {}

void JPlayerAnimator::Initialize()
{
	JAnimator::Initialize();

	AddAnimationClip("Idle_Free",
					 GetWorld.AnimationManager->Load("Game/Animation/Anim_Hands_Empty_Idle.jasset",
													 mSkeletalMeshComponent));
	AddAnimationClip("Walk_Free",
					 GetWorld.AnimationManager->Load("Game/Animation/Anim_Hands_Empty_Walk.jasset",
													 mSkeletalMeshComponent));
	// AddAnimationClip("Run_Free", );
	//
	// AddAnimationClip("Idle_OneHand_Pistol", );
	// AddAnimationClip("Walk_OneHand_Pistol", );
	// AddAnimationClip("Run_OneHand_Pistol", );
}
