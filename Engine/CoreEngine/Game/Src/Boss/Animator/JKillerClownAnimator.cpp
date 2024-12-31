#include "JKillerClownAnimator.h"

#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"
#include "Core/Interface/JWorld.h"
#include "Game/Src/Boss/ABoss.h"

JKillerClownAnimator::JKillerClownAnimator()
{
}

JKillerClownAnimator::JKillerClownAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp)
    : JAnimator(InName, InSkeletalComp)
{
    mBoss = dynamic_cast<ABoss*>(InSkeletalComp->GetOwnerActor());
    assert(mBoss);
}

void JKillerClownAnimator::Initialize()
{
    JAnimator::Initialize();

    AddAnimationClip("Idle",
                     "Game/Animation/KillerClown/BZ_Idle.jasset");
    AddAnimationClip("Walk",
                     "Game/Animation/KillerClown/BZ_Run.jasset");
    AddAnimationClip("Death",
                     "Game/Animation/KillerClown/BZ_Death_Spinning.jasset");
    AddAnimationClip("Attack",
                     "Game/Animation/KillerClown/Zombie_Attack_4.jasset"); //Zombie_Attack_01

    mStateMachine["Death"]->OnAnimFinished.Bind([this]()
    {
        if (mBoss)
            mBoss->Destroy();
    });

    auto& attackClip = mStateMachine["Attack"];
    attackClip->SetLoop(false);
    attackClip->SetAnimationSpeed(2.f);
    attackClip->mEvents[attackClip->GetEndFrame() * 0.7].Bind([&]()
    {
        if (mBoss)
        {
            // mBoss->AddLocalLocation(attackClip->GetRMPosition());
            mBoss->SetEnemyState(EBossState::Idle);
        }
    });

    AddAnimLink("Idle", "Walk", [&]() { return !mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
    AddAnimLink("Walk", "Idle", [&]() { return mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);

    AddAnimLink("Idle", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    AddAnimLink("Walk", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    AddAnimLink("Attack", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);

    AddAnimLink("Idle", "Attack", [&]() { return mBoss->mBossState == EBossState::Attack; }, 0.5f);
    AddAnimLink("Walk", "Attack", [&]() { return mBoss->mBossState == EBossState::Attack; }, 0.5f);

    AddAnimLink("Attack", "Idle", [&]() { return mBoss->mBossState == EBossState::Idle; }, 0.5f);
    // AddAnimLink("Attack", "Walk", [&](){return !mMovementComponent->GetVelocity().IsNearlyZero() && /*공격 끝난상태*/});


    SetState("Idle");
    mCurrentAnimation->Play();
}

void JKillerClownAnimator::BeginPlay()
{
    // JAnimator::BeginPlay();
}
