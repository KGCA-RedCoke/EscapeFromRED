#include "JButcherAnimator.h"

#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Component/AI/BT_Butcher.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"
#include "Core/Interface/JWorld.h"
#include "Game/Src/Enemy/AEnemy.h"

JButcherAnimator::JButcherAnimator()
{
}

JButcherAnimator::JButcherAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp)
    : JAnimator(InName, InSkeletalComp)
{
    mEnemy = dynamic_cast<AEnemy*>(InSkeletalComp->GetOwnerActor());
    assert(mEnemy);
}

void JButcherAnimator::Initialize()
{
    JAnimator::Initialize();
    bt = dynamic_cast<BT_Butcher*>(mEnemy->mBehaviorTree);
    AddAnimationClip("Idle",
                     "Game/Animation/Butcher/ThirdPersonIdle.jasset", true);
    AddAnimationClip("Walk",
                     "Game/Animation/Butcher/ThirdPersonWalk.jasset", true);
    AddAnimationClip("Convers1",
                     "Game/Animation/Butcher/Convo_01_Low_Key_Loop.jasset", true);
    AddAnimationClip("Convers2",
                     "Game/Animation/Butcher/Convo_02_Up_Beat_Loop.jasset", true);
    AddAnimationClip("Convers3",
                     "Game/Animation/Butcher/Convo_03_Animated_Loop.jasset", true);
    AddAnimationClip("Convers4",
                     "Game/Animation/Butcher/Convo_04_Very_Animated_Loop.jasset", true);
    AddAnimationClip("ConversSad",
                     "Game/Animation/Butcher/Convo_07_Sad_Loop.jasset", true);
    // AddAnimationClip("Death",
    //                  "Game/Animation/Butcher/BZ_Death_Spinning.jasset", false);
    // AddAnimationClip("Attack",
    //                  "Game/Animation/Butcher/Zombie_Attack_4.jasset", false); //Zombie_Attack_01

    // mStateMachine["Death"]->OnAnimFinished.Bind([this]()
    // {
    //     if (mEnemy)
    //         mEnemy->Destroy();
    // });
    //


    AddAnimLink("Idle", "Walk", [&]() { return !mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
    AddAnimLink("Walk", "Idle", [&]() { return mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);

    AddAnimLink("Idle", "Convers1", [&](){ return mEnemy->mEnemyState == EEnemyState::Convers1;  }, 0.5f);
    AddAnimLink("Convers1", "Convers2", [&](){ return mEnemy->mEnemyState == EEnemyState::Convers2; }, 0.5f);
    AddAnimLink("Convers2", "Convers3", [&](){ return mEnemy->mEnemyState == EEnemyState::Convers3; }, 0.5f);
    AddAnimLink("Convers3", "Convers4", [&](){ return mEnemy->mEnemyState == EEnemyState::Convers4; }, 0.5f);
    AddAnimLink("Convers4", "Idle", [&](){ return mEnemy->mEnemyState == EEnemyState::Idle; }, 0.5f);
    
    // AddAnimLink("Walk", "Death", [&]() { return mEnemy->mEnemyState == EEnemyState::Death; }, 0.5f);
    // AddAnimLink("Attack", "Death", [&]() { return mEnemy->mEnemyState == EEnemyState::Death; }, 0.5f);

    // AddAnimLink("Idle", "Attack", [&]() { return mEnemy->mEnemyState == EEnemyState::Attack; }, 0.5f);
    // AddAnimLink("Walk", "Attack", [&]() { return mEnemy->mEnemyState == EEnemyState::Attack; }, 0.5f);
    //
    // AddAnimLink("Attack", "Idle", [&]() { return mEnemy->mEnemyState == EEnemyState::Idle; }, 0.5f);
    // AddAnimLink("Attack", "Walk", [&](){return !mMovementComponent->GetVelocity().IsNearlyZero() && /*공격 끝난상태*/});


    SetState("Idle");
    mCurrentAnimation->Play();
}

void JButcherAnimator::BeginPlay()
{
    JAnimator::BeginPlay();
}
