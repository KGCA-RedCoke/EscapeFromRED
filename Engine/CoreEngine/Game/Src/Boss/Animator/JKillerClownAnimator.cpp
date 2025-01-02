#include "JKillerClownAnimator.h"

#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"
#include "Core/Interface/JWorld.h"
#include "Game/Src/Boss/AKillerClown.h"
#include "Core/Entity/Component/AI/BT_BOSS.h"

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
    bt = dynamic_cast<BT_BOSS*>(mBoss->mBehaviorTree);
    AddAnimationClip("Idle",
                     "Game/Animation/KillerClown/KC_Idle.jasset");
    AddAnimationClip("Walk",
                     "Game/Animation/KillerClown/KC_Walk.jasset");
    AddAnimationClip("Run",
                     "Game/Animation/KillerClown/KC_Run.jasset");
    AddAnimationClip("Attack1",
                     "Game/Animation/KillerClown/KC_WeaponSwing.jasset");
    AddAnimationClip("Attack2",
                     "Game/Animation/KillerClown/KC_Attack_02.jasset");
    AddAnimationClip("Attack3",
                     "Game/Animation/KillerClown/KC_Attack_01.jasset");
    AddAnimationClip("JumpAttack",
                     "Game/Animation/KillerClown/Anim_KC_JumpAttack.jasset");
    AddAnimationClip("Hit",
                     "Game/Animation/KillerClown/KC_Hit.jasset");
    AddAnimationClip("Death",
                     "Game/Animation/KillerClown/KC_Die.jasset");
    AddAnimationClip("StandUp",
                     "Game/Animation/KillerClown/KC_Standup.jasset");

    mStateMachine["Death"]->OnAnimFinished.Bind([this]()
    {
        if (mBoss && bt->mPhase > 2)
            mBoss->Destroy();
        else
        {
            bt->mPhase++;
        }
    });

    auto& hitClip = mStateMachine["Hit"];
    hitClip->SetLoop(false);
    hitClip->OnAnimFinished.Bind([&]()
    {
        if (mBoss)
        {
            // mBoss->AddLocalLocation(attackClip->GetRMPosition());
            mBoss->SetEnemyState(EBossState::Attack2);
        }
    });
    
    auto& attackClip1 = mStateMachine["Attack1"];
    attackClip1->SetLoop(false);
    attackClip1->SetAnimationSpeed(2.f);
    attackClip1->mEvents[attackClip1->GetEndFrame() * 0.7].Bind([&]()
    {
        if (mBoss)
        {
            // mBoss->AddLocalLocation(attackClip->GetRMPosition());
            mBoss->SetEnemyState(EBossState::Idle);
        }
    });

    auto& attackClip2 = mStateMachine["Attack2"];
    attackClip2->SetLoop(false);
    attackClip2->SetAnimationSpeed(1.5f);
    // attackClip2->mEvents[attackClip2->GetEndFrame()].Bind([&]()
    attackClip2->OnAnimFinished.Bind([&]()
    {
        if (mBoss)
        {
            // mBoss->AddLocalLocation(attackClip->GetRMPosition());
            mBoss->SetEnemyState(EBossState::Idle);
        }
    });

    auto& attackClip3 = mStateMachine["Attack3"];
    attackClip3->SetLoop(false);
    attackClip3->SetAnimationSpeed(2.f);
    attackClip3->mEvents[attackClip3->GetEndFrame() * 0.7].Bind([&]()
    {
        if (mBoss)
        {
            // mBoss->AddLocalLocation(attackClip->GetRMPosition());
            mBoss->SetEnemyState(EBossState::Idle);
        }
    });
    
    auto& attackClip4 = mStateMachine["JumpAttack"];
    attackClip4->SetLoop(false);
    attackClip4->SetAnimationSpeed(1.5f);
    attackClip4->OnAnimFinished.Bind([&]()
    {
        if (mBoss)
        {
            // mBoss->AddLocalLocation(attackClip->GetRMPosition());
            mBoss->SetEnemyState(EBossState::Idle);
        }
    });

    auto& DeathClip = mStateMachine["Death"];
    DeathClip->SetLoop(false);
    // DeathClip->SetAnimationSpeed(1.5f);
    DeathClip->OnAnimFinished.Bind([&]()
    {
        if (mBoss)
        {
            // mBoss->AddLocalLocation(attackClip->GetRMPosition());
            mBoss->SetEnemyState(EBossState::StandUp);
        }
    });
    
    AddAnimLink("Idle", "Walk", [&]() { return !mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
    AddAnimLink("Walk", "Idle", [&]() { return mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);

    AddAnimLink("Idle", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    AddAnimLink("Walk", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    
    AddAnimLink("Attack1", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    AddAnimLink("Idle", "Attack1", [&]() { return mBoss->mBossState == EBossState::Attack1; }, 0.5f);
    AddAnimLink("Walk", "Attack1", [&]() { return mBoss->mBossState == EBossState::Attack1; }, 0.5f);
    AddAnimLink("Attack1", "Idle", [&]() { return mBoss->mBossState == EBossState::Idle; }, 0.5f);
    AddAnimLink("Attack1", "Walk", [&]() { return mBoss->mBossState == EBossState::Idle; }, 0.5f);
    
    AddAnimLink("Attack2", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    AddAnimLink("Idle", "Attack2", [&]() { return mBoss->mBossState == EBossState::Attack2; }, 0.5f);
    AddAnimLink("Walk", "Attack2", [&]() { return mBoss->mBossState == EBossState::Attack2; }, 0.5f);
    AddAnimLink("Attack2", "Idle", [&]() { return mBoss->mBossState == EBossState::Idle; }, 0.5f);
    AddAnimLink("Attack2", "Walk", [&]() { return mBoss->mBossState == EBossState::Idle; }, 0.5f);
    
    AddAnimLink("Attack3", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    AddAnimLink("Idle", "Attack3", [&]() { return mBoss->mBossState == EBossState::Attack3; }, 0.5f);
    AddAnimLink("Walk", "Attack3", [&]() { return mBoss->mBossState == EBossState::Attack3; }, 0.5f);
    AddAnimLink("Attack3", "Idle", [&]() { return mBoss->mBossState == EBossState::Idle; }, 0.5f);
    AddAnimLink("Attack3", "Walk", [&]() { return mBoss->mBossState == EBossState::Idle; }, 0.5f);
    
    AddAnimLink("JumpAttack", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    AddAnimLink("Idle", "JumpAttack", [&]() { return mBoss->mBossState == EBossState::JumpAttack; }, 0.5f);
    AddAnimLink("Walk", "JumpAttack", [&]() { return mBoss->mBossState == EBossState::JumpAttack; }, 0.5f);
    AddAnimLink("JumpAttack", "Idle", [&]() { return mBoss->mBossState == EBossState::Idle; }, 0.5f);
    AddAnimLink("JumpAttack", "Walk", [&]() { return mBoss->mBossState == EBossState::Idle; }, 0.5f);
    
    AddAnimLink("JumpAttack", "Walk", [&]() { return mBoss->mBossState == EBossState::Idle; }, 0.5f);
    // AddAnimLink("Attack", "Walk", [&](){return !mMovementComponent->GetVelocity().IsNearlyZero() && /*공격 끝난상태*/});
    AddAnimLink("Death", "StandUp", [&]() { return mBoss->mBossState == EBossState::StandUp; }, 0.5f);
    AddAnimLink("StandUp", "Idle", [&]() { return mBoss->mBossState == EBossState::Idle; }, 0.5f);
    


    SetState("Idle");
    mCurrentAnimation->Play();
}

void JKillerClownAnimator::BeginPlay()
{
    JAnimator::BeginPlay();
}

void JKillerClownAnimator::Tick(float DeltaTime)
{
    JAnimator::Tick(DeltaTime);
}
