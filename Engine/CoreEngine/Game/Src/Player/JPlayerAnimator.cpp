#include "JPlayerAnimator.h"

#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"
#include "Core/Interface/JWorld.h"

JPlayerAnimator::JPlayerAnimator() {}

JPlayerAnimator::JPlayerAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp)
	: JAnimator(InName, InSkeletalComp) {}

JPlayerAnimator::JPlayerAnimator(JTextView InName, AActor* InOwnerActor)
	: JAnimator(InName) {}

void JPlayerAnimator::Initialize()
{
	JAnimator::Initialize();

	mMovementComponent = static_cast<JPawnMovementComponent*>(mSkeletalMeshComponent->GetOwnerActor()->
		GetChildComponentByType(NAME_COMPONENT_PAWN_MOVEMENT));

	AddAnimationClip("Idle_Free",
					 GetWorld.AnimationManager->Load("Game/Animation/Anim_Hands__Lantern_01_Walk.jasset",
													 mSkeletalMeshComponent));
	AddAnimationClip("Walk_Free",
					 GetWorld.AnimationManager->Load("Game/Animation/Anim_Hands__Lantern_01_Run.jasset",
													 mSkeletalMeshComponent));


	AddAnimLink("Idle_Free", "Walk_Free", [&](){ return !mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
	AddAnimLink("Walk_Free", "Idle_Free", [&](){ return mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);

	SetState("Idle_Free");
	mCurrentAnimation->Play();
}

void JPlayerAnimator::BeginPlay()
{
	JAnimator::BeginPlay();
}
