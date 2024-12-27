#include "JKihyunAnimator.h"

#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"
#include "Core/Interface/JWorld.h"

JKihyunAnimator::JKihyunAnimator()
{
}

JKihyunAnimator::JKihyunAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp)
    : JAnimator(InName, InSkeletalComp)
{
}

JKihyunAnimator::JKihyunAnimator(JTextView InName, AActor* InOwnerActor)
    : JAnimator(InName)
{
}

void JKihyunAnimator::Initialize()
{
    JAnimator::Initialize();



    AddAnimationClip("Idle",
                     GetWorld.AnimationManager->Load("Game/Animation/Anim_Zombie_Idle.jasset",
                     mSkeletalMeshComponent));
    AddAnimationClip("Walk",
                     GetWorld.AnimationManager->Load("Game/Animation/Anim_Zombie_Run_02.jasset",
                     mSkeletalMeshComponent));
    AddAnimationClip("Attack",
                     GetWorld.AnimationManager->Load("Game/Animation/Anim_Zombie_Jump_Idle.jasset",
                                                     mSkeletalMeshComponent));


    AddAnimLink("Idle", "Walk", [&]() { return !mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
    AddAnimLink("Walk", "Idle", [&]() { return mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);

    SetState("Idle");
    mCurrentAnimation->Play();
}

void JKihyunAnimator::BeginPlay()
{
    JAnimator::BeginPlay();
}
