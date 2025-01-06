#include "JPigAnimator.h"

#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"
#include "Core/Interface/JWorld.h"
#include "Game/Src/Enemy/AEnemy.h"

JPigAnimator::JPigAnimator()
{
}

JPigAnimator::JPigAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp)
    : JAnimator(InName, InSkeletalComp)
{
    mEnemy = dynamic_cast<AEnemy*>(InSkeletalComp->GetOwnerActor());
    assert(mEnemy);
}

void JPigAnimator::Initialize()
{
    JAnimator::Initialize();

    AddAnimationClip("Idle",
                     "Game/Animation/Pig/Pig_Idle.jasset", true);
    AddAnimationClip("Walk",
                     "Game/Animation/Pig/Pig_Run_Forward.jasset", true);
    // AddAnimationClip("Attack",
    //                  "Game/Animation/BigZombie/Zombie_Attack_4.jasset", false); //Zombie_Attack_01

    auto& walkClip = mStateMachine["Walk"];
    walkClip->SetAnimationSpeed(1.5f);
    
    AddAnimLink("Idle", "Walk", [&]() { return !mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
    AddAnimLink("Walk", "Idle", [&]() { return mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);


    // AddAnimLink("Attack", "Walk", [&](){return !mMovementComponent->GetVelocity().IsNearlyZero() && /*공격 끝난상태*/});


    SetState("Idle");
    mCurrentAnimation->Play();
}

void JPigAnimator::BeginPlay()
{
    JAnimator::BeginPlay();
}
