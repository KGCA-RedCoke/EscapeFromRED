﻿#include "JKillerClownAnimator.h"

#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Audio/MSoundManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"
#include "Core/Interface/JWorld.h"
#include "Game/Src/Boss/AKillerClown.h"
#include "Core/Entity/Component/AI/BT_BOSS.h"
#include "Core/Entity/Navigation/BigGrid.h"
#include "Core/Entity/Component/Mesh/JStaticMeshComponent.h"

#define RADIUS 8.f

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

    mLaughSound = GetWorld.SoundManager->Load("rsc/GameResource/Sound/LaughSound.wav");
    
    bt = dynamic_cast<BT_BOSS*>(mBoss->mBehaviorTree);
    AddAnimationClip("Idle",
                     "Game/Animation/KillerClown/KC_Idle.jasset", true);
    AddAnimationClip("Walk",
                     "Game/Animation/KillerClown/KC_Walk.jasset", true);
    AddAnimationClip("Run",
                     "Game/Animation/KillerClown/KC_Run.jasset", true);
    AddAnimationClip("Attack1",
                     "Game/Animation/KillerClown/KC_WeaponSwing.jasset");
    AddAnimationClip("Attack2",
                     "Game/Animation/KillerClown/KC_Attack_02.jasset");
    AddAnimationClip("Attack3",
                     "Game/Animation/KillerClown/KC_Attack_01.jasset");
    AddAnimationClip("Attack4",
                     "Game/Animation/KillerClown/KC_Attack.jasset");
    AddAnimationClip("JumpAttack",
                     "Game/Animation/KillerClown/Anim_KC_JumpAttack.jasset"); //Anim_KC_JumpAttack
    AddAnimationClip("Hit",
                     "Game/Animation/KillerClown/KC_Hit.jasset");
    AddAnimationClip("Death",
                     "Game/Animation/KillerClown/KC_Die.jasset");
    AddAnimationClip("StandUp",
                     "Game/Animation/KillerClown/KC_Standup.jasset");

    mStateMachine["Idle"]->OnAnimStart.Bind([&]()
    {
        if (mBoss)
        {
            mBoss->mWeaponCollider->SetLocalScale(FVector(1.0f, 1.0f, 1.0f));
            mBoss->DisableAttackCollision();
        }
    });

    auto& hitClip = mStateMachine["Hit"];
    hitClip->OnAnimStart.Bind([&]()
    {
        if (mBoss)
        {
            mBoss->HP -= 34;
            if (mBoss->HP <= 0)
            {
                mBoss->SetBossState(EBossState::Death);
                bt->mPhase++;
            }
        }
    }); //            

    hitClip->mEvents[hitClip->GetEndFrame() * 0.8].Bind([&]()
    {
        if (mBoss)
        {
            mBoss->SetBossState(EBossState::Idle);
        }
    });

    auto& attackClip1 = mStateMachine["Attack1"];
    attackClip1->SetAnimationSpeed(2.f);
    attackClip1->mEvents[attackClip1->GetEndFrame() * 0.3].Bind(std::bind(&AEnemy::EnableAttackCollision, mBoss, 1));
    attackClip1->mEvents[attackClip1->GetEndFrame() * 0.6].Bind(std::bind(&AEnemy::DisableAttackCollision, mBoss));
    attackClip1->mEvents[attackClip1->GetEndFrame() * 0.7].Bind([&]()
    {
        if (mBoss)
        {
            mBoss->SetBossState(EBossState::Idle);
        }
    });

    auto& attackClip2 = mStateMachine["Attack2"];
    attackClip2->SetAnimationSpeed(1.5f);
    attackClip2->mEvents[attackClip2->GetEndFrame() * 0.2].Bind(std::bind(&AEnemy::EnableAttackCollision, mBoss, 1));
    attackClip2->mEvents[attackClip2->GetEndFrame() * 0.7].Bind(std::bind(&AEnemy::DisableAttackCollision, mBoss));
    attackClip2->mEvents[attackClip2->GetEndFrame() * 0.8].Bind([&]()
    {
        if (mBoss)
        {
            mBoss->SetBossState(EBossState::Idle);
        }
    });

    auto& attackClip3 = mStateMachine["Attack3"];
    attackClip3->SetAnimationSpeed(2.f);
    attackClip3->SetRootMotion(true);
    attackClip3->mEvents[attackClip3->GetEndFrame() * 0.4].Bind(std::bind(&AEnemy::EnableAttackCollision, mBoss, 1));
    attackClip3->mEvents[attackClip3->GetEndFrame() * 0.8].Bind(std::bind(&AEnemy::DisableAttackCollision, mBoss));
    attackClip3->mEvents[attackClip3->GetEndFrame() * 0.9].Bind([&]()
    {
        if (mBoss)
        {
            mBoss->SetBossState(EBossState::Idle);
        }
    });

    auto& attackClip4 = mStateMachine["Attack4"];
    attackClip4->SetAnimationSpeed(1.f);
    attackClip4->SetRootMotion(true);
    attackClip4->mEvents[attackClip4->GetEndFrame() * 0.2].Bind(std::bind(&AEnemy::EnableAttackCollision, mBoss, 1));
    attackClip4->mEvents[attackClip4->GetEndFrame() * 0.3].Bind(std::bind(&AEnemy::DisableAttackCollision, mBoss));
    attackClip4->mEvents[attackClip4->GetEndFrame() * 0.7].Bind([&]()
    {
        if (mBoss)
        {
            mBoss->SetBossState(EBossState::Idle);
        }
    });

    auto& JumpAtkClip = mStateMachine["JumpAttack"];
    JumpAtkClip->SetAnimationSpeed(1.3f);
    JumpAtkClip->SetRootMotion(true);
    JumpAtkClip->OnAnimStart.Bind(std::bind(&AEnemy::DisableAttackCollision, mBoss));

    JumpAtkClip->mEvents[JumpAtkClip->GetEndFrame() * 0.55].Bind(
        std::bind(&AEnemy::EnableAttackCollision, mBoss, RADIUS));
    JumpAtkClip->mEvents[JumpAtkClip->GetEndFrame() * 0.65].Bind(std::bind(&AEnemy::DisableAttackCollision, mBoss));
    JumpAtkClip->mEvents[JumpAtkClip->GetEndFrame() * 0.9].Bind([&]()
    {
        if (mBoss)
        {
            mBoss->SetBossState(EBossState::Idle);
        }
    });

    auto& DeathClip = mStateMachine["Death"];
    DeathClip->mEvents[DeathClip->GetEndFrame() * 0.9].Bind([&]()
    {
        if (mBoss)
        {
            if (bt->mPhase == 3)
            {
                mBoss->DisableAttackCollision();
                mBoss->mCollisionSphere->Destroy();
                mBoss->mWeaponCollider->Destroy();
                mBoss->Destroy();
            }
            DeathClip->Pause();
        }
    });
    DeathClip->OnAnimFinished.Bind([&]()
    {
        if (mBoss)
        {
            mBoss->SetBossState(EBossState::StandUp);
            mLaughSound->Play();
        }
    });

    auto& StandUpClip = mStateMachine["StandUp"];
    StandUpClip->SetAnimationSpeed(1.5f);
    StandUpClip->OnAnimStart.Bind([&]()
    {
        if (mBoss)
        {
            mBoss->HP = 100;
        }
    });

    StandUpClip->mEvents[StandUpClip->GetEndFrame() * 0.9].Bind([&]()
    {
        if (mBoss)
        {
            mBoss->SetBossState(EBossState::Idle);
        }
    });
    AddAnimLink("Idle", "Walk", [&]() { return !bt->IsRun && !mMovementComponent->GetVelocity().IsNearlyZero(); },
                0.2f);
    AddAnimLink("Idle", "Run", [&]() { return bt->IsRun && !mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
    AddAnimLink("Walk", "Idle", [&]() { return !bt->IsRun && mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
    AddAnimLink("Run", "Idle", [&]() { return bt->IsRun && mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);

    AddAnimLink("Idle", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    AddAnimLink("Walk", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    AddAnimLink("Run", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    AddAnimLink("Attack1", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    AddAnimLink("Attack2", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    AddAnimLink("Attack3", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    AddAnimLink("JumpAttack", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);
    AddAnimLink("Hit", "Death", [&]() { return mBoss->mBossState == EBossState::Death; }, 0.5f);

    AddAnimLink("Hit", "Idle", [&]() { return mBoss->mBossState == EBossState::Idle; }, 0.5f);
    AddAnimLink("Idle", "Hit", [&]() { return mBoss->mBossState == EBossState::Hit; }, 0.5f);
    AddAnimLink("Walk", "Hit", [&]() { return mBoss->mBossState == EBossState::Hit; }, 0.5f);
    AddAnimLink("Run", "Hit", [&]() { return mBoss->mBossState == EBossState::Hit; }, 0.5f);
    AddAnimLink("Attack1", "Hit", [&]() { return mBoss->mBossState == EBossState::Hit; }, 0.5f);
    AddAnimLink("Attack2", "Hit", [&]() { return mBoss->mBossState == EBossState::Hit; }, 0.5f);
    AddAnimLink("Attack3", "Hit", [&]() { return mBoss->mBossState == EBossState::Hit; }, 0.5f);
    AddAnimLink("Attack4", "Hit", [&]() { return mBoss->mBossState == EBossState::Hit; }, 0.5f);
    AddAnimLink("JumpAttack", "Hit", [&]() { return mBoss->mBossState == EBossState::Hit; }, 0.5f);

    AddAnimLink("Idle", "Attack1", [&]() { return mBoss->mBossState == EBossState::Attack1; }, 0.5f);
    AddAnimLink("Walk", "Attack1", [&]() { return mBoss->mBossState == EBossState::Attack1; }, 0.5f);
    AddAnimLink("Run", "Attack1", [&]() { return mBoss->mBossState == EBossState::Attack1; }, 0.5f);
    AddAnimLink("Hit", "Attack1", [&]() { return mBoss->mBossState == EBossState::Attack1; }, 0.5f);
    AddAnimLink("Attack1", "Idle", [&]() 
    {
        return mBoss->mBossState == EBossState::Idle | GetAnimElapsedRatio() > 0.89f;
    }, 0.5f);
    AddAnimLink("Attack1", "Walk", [&]() { return mBoss->mBossState == EBossState::Walk; }, 0.5f);
    AddAnimLink("Attack1", "Run", [&]() { return mBoss->mBossState == EBossState::Run; }, 0.5f);

    AddAnimLink("Idle", "Attack2", [&]() { return mBoss->mBossState == EBossState::Attack2; }, 0.5f);
    AddAnimLink("Walk", "Attack2", [&]() { return mBoss->mBossState == EBossState::Attack2; }, 0.5f);
    AddAnimLink("Run", "Attack2", [&]() { return mBoss->mBossState == EBossState::Attack2; }, 0.5f);
    AddAnimLink("Hit", "Attack2", [&]() { return mBoss->mBossState == EBossState::Attack2; }, 0.5f);
    AddAnimLink("Attack2", "Idle", [&]()
    {
        return mBoss->mBossState == EBossState::Idle | GetAnimElapsedRatio() > 0.89f;
    }, 0.5f);
    AddAnimLink("Attack2", "Walk", [&]() { return mBoss->mBossState == EBossState::Walk; }, 0.5f);
    AddAnimLink("Attack2", "Run", [&]() { return mBoss->mBossState == EBossState::Run; }, 0.5f);

    AddAnimLink("Idle", "Attack3", [&]() { return mBoss->mBossState == EBossState::Attack3; }, 0.5f);
    AddAnimLink("Walk", "Attack3", [&]() { return mBoss->mBossState == EBossState::Attack3; }, 0.5f);
    AddAnimLink("Run", "Attack3", [&]() { return mBoss->mBossState == EBossState::Attack3; }, 0.5f);
    AddAnimLink("Hit", "Attack3", [&]() { return mBoss->mBossState == EBossState::Attack3; }, 0.5f);
    AddAnimLink("Attack3", "Idle", [&]() { return mBoss->mBossState == EBossState::Idle; }, 0.5f);
    AddAnimLink("Attack3", "Walk", [&]() { return mBoss->mBossState == EBossState::Walk; }, 0.5f);
    AddAnimLink("Attack3", "Run", [&]() { return mBoss->mBossState == EBossState::Run; }, 0.5f);

    AddAnimLink("Idle", "Attack4", [&]() { return mBoss->mBossState == EBossState::Attack4; }, 0.5f);
    AddAnimLink("Walk", "Attack4", [&]() { return mBoss->mBossState == EBossState::Attack4; }, 0.5f);
    AddAnimLink("Run", "Attack4", [&]() { return mBoss->mBossState == EBossState::Attack4; }, 0.5f);
    AddAnimLink("Hit", "Attack4", [&]() { return mBoss->mBossState == EBossState::Attack4; }, 0.5f);
    AddAnimLink("Attack4", "Idle", [&]() { return mBoss->mBossState == EBossState::Idle; }, 0.5f);
    AddAnimLink("Attack4", "Walk", [&]() { return mBoss->mBossState == EBossState::Walk; }, 0.5f);
    AddAnimLink("Attack4", "Run", [&]() { return mBoss->mBossState == EBossState::Run; }, 0.5f);
    AddAnimLink("Hit", "Attack4", [&]() { return mBoss->mBossState == EBossState::Attack4; }, 0.5f);

    AddAnimLink("Idle", "JumpAttack", [&]() { return mBoss->mBossState == EBossState::JumpAttack; }, 0.5f);
    AddAnimLink("Walk", "JumpAttack", [&]() { return mBoss->mBossState == EBossState::JumpAttack; }, 0.5f);
    AddAnimLink("Run", "JumpAttack", [&]() { return mBoss->mBossState == EBossState::JumpAttack; }, 0.5f);
    AddAnimLink("Hit", "JumpAttack", [&]() { return mBoss->mBossState == EBossState::JumpAttack; }, 0.5f);
    AddAnimLink("JumpAttack", "Idle", [&]() { return mBoss->mBossState == EBossState::Idle; }, 0.5f);
    AddAnimLink("JumpAttack", "Walk", [&]() { return mBoss->mBossState == EBossState::Walk; }, 0.5f);
    AddAnimLink("JumpAttack", "Run", [&]() { return mBoss->mBossState == EBossState::Run; }, 0.5f);

    // AddAnimLink("Attack", "Walk", [&](){return !mMovementComponent->GetVelocity().IsNearlyZero() && /*공격 끝난상태*/});

    AddAnimLink("Death", "StandUp", [&]() { return mBoss->mBossState == EBossState::StandUp; }, 0.5f);
    AddAnimLink("StandUp", "Idle", [&]()
    {
        return (mBoss->mBossState == EBossState::Idle)
            || GetAnimElapsedRatio() > 0.9f;
    }, 0.5f);


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
    if (bt->bIsStandUpReady)
    {
        mBoss->mHammerMesh->AttachToBoneSocket(mSkeletalMeshComponent, "Wrist_L");
        mBoss->mHammerMesh->SetLocalLocation({91, 153, 9});
        mBoss->mHammerMesh->SetLocalRotation({68.2f, 79.9f, -91.1f});
        mStateMachine["Death"]->Resume();
    }
}
