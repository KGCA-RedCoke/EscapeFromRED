#include "JKihyunAnimator.h"

#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"
#include "Core/Interface/JWorld.h"

JKihyunAnimator::JKihyunAnimator()
{}

JKihyunAnimator::JKihyunAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp)
	: JAnimator(InName, InSkeletalComp)
{}

JKihyunAnimator::JKihyunAnimator(JTextView InName, AActor* InOwnerActor)
	: JAnimator(InName)
{}

void JKihyunAnimator::Initialize()
{
	JAnimator::Initialize();

	AddAnimationClip("Idle",
					 GetWorld.AnimationManager->Load("Game/Animation/BigZombie/BZ_Idle.jasset",
													 mSkeletalMeshComponent));
	AddAnimationClip("Walk",
					 GetWorld.AnimationManager->Load("Game/Animation/BigZombie/BZ_Run.jasset",
													 mSkeletalMeshComponent));
	AddAnimationClip("Death",
					 GetWorld.AnimationManager->Load("Game/Animation/BigZombie/BZ_Death_Spinning.jasset",
													 mSkeletalMeshComponent));

	AddAnimLink("Idle", "Walk", [&](){ return !mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
	AddAnimLink("Walk", "Idle", [&](){ return mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
	

	SetState("Idle");
	mCurrentAnimation->Play();
}

void JKihyunAnimator::BeginPlay()
{
	JAnimator::BeginPlay();
}
