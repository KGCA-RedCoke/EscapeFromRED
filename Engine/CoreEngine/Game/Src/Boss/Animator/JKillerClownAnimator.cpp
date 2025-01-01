#include "JKillerClownAnimator.h"

#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"
#include "Core/Interface/JWorld.h"
#include "Game/Src/Boss/AKillerClown.h"

JKillerClownAnimator::JKillerClownAnimator()
{
}

JKillerClownAnimator::JKillerClownAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp)
    : JAnimator(InName, InSkeletalComp)
{
    mBoss = dynamic_cast<AKillerClown*>(InSkeletalComp->GetOwnerActor());
    assert(mBoss);
}

void JKillerClownAnimator::Initialize()
{
    JAnimator::Initialize();

    // AddAnimationClip("Idle",
    //                  "Game/Animation/KillerClown/KC_Idle.jasset");
    // AddAnimationClip("Walk",
    //                  "Game/Animation/KillerClown/KC_Walk.jasset");
    // AddAnimationClip("Run",
    //                  "Game/Animation/KillerClown/KC_Die.jasset");
    // AddAnimationClip("Attack1",
    //                  "Game/Animation/KillerClown/KC_WeaponSwing.jasset");
    // AddAnimationClip("Attack2",
    //                  "Game/Animation/KillerClown/KC_Attack_02.jasset");
    // AddAnimationClip("Attack3",
    //                  "Game/Animation/KillerClown/KC_Attack_01.jasset");
    // AddAnimationClip("Hit",
    //                  "Game/Animation/KillerClown/KC_Run.jasset");
    // AddAnimationClip("Death",
    //                  "Game/Animation/KillerClown/KC_Hit.jasset");
    // AddAnimationClip("StandUp",
    //                  "Game/Animation/KillerClown/KC_Standup.jasset");
    //
    // mStateMachine["Death"]->OnAnimFinished.Bind([this]()
    // {
    //     if (mBoss)
    //         mBoss->Destroy();
    // });
    //
    // auto& attackClip = mStateMachine["Attack1"];
    // attackClip->SetLoop(false);
    // attackClip->SetAnimationSpeed(2.f);
    // attackClip->mEvents[attackClip->GetEndFrame() * 0.7].Bind([&]()
    // {
    //     if (mBoss)
    //     {
    //         // mBoss->AddLocalLocation(attackClip->GetRMPosition());
    //         mBoss->SetEnemyState(EBossState::Idle);
    //     }
    // });
    //
    // AddAnimLink("Idle", "Walk", [&]() { return !mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
    // AddAnimLink("Walk", "Idle", [&]() { return mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
    //
    // AddAnimLink("Idle", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    // AddAnimLink("Walk", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    // AddAnimLink("Attack1", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    //
    // AddAnimLink("Idle", "Attack1", [&]() { return mBoss->mBossState == EBossState::Attack1; }, 0.5f);
    // AddAnimLink("Walk", "Attack1", [&]() { return mBoss->mBossState == EBossState::Attack1; }, 0.5f);
    //
    // AddAnimLink("Attack1", "Idle", [&]() { return mBoss->mBossState == EBossState::Idle; }, 0.5f);
    // // AddAnimLink("Attack", "Walk", [&](){return !mMovementComponent->GetVelocity().IsNearlyZero() && /*공격 끝난상태*/});
    //
    //
    // SetState("Idle");
    // mCurrentAnimation->Play();
}

void JKillerClownAnimator::BeginPlay()
{
    JAnimator::BeginPlay();
}
