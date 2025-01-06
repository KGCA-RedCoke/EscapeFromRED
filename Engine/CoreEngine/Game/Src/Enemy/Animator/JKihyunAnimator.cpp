#include "JKihyunAnimator.h"

#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"
#include "Core/Interface/JWorld.h"
#include "Game/Src/Enemy/AEnemy.h"

JKihyunAnimator::JKihyunAnimator()
{
}

JKihyunAnimator::JKihyunAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp)
    : JAnimator(InName, InSkeletalComp)
{
    mEnemy = dynamic_cast<AEnemy*>(InSkeletalComp->GetOwnerActor());
    assert(mEnemy);
}

void JKihyunAnimator::Initialize()
{
    JAnimator::Initialize();

    AddAnimationClip("Idle",
                     "Game/Animation/BigZombie/BZ_Idle.jasset", true);
    AddAnimationClip("Walk",
                     "Game/Animation/BigZombie/BZ_Run.jasset", true);
    AddAnimationClip("Death",
                     "Game/Animation/BigZombie/BZ_Death_Spinning.jasset", false);
    AddAnimationClip("Attack",
                     "Game/Animation/BigZombie/Zombie_Attack_4.jasset", false); //Zombie_Attack_01

    
    mStateMachine["Idle"]->OnAnimStart.Bind([&]()
    {
        if (mEnemy)
        {
            // mEnemy->mWeaponCollider->SetLocalScale(FVector(1.0f, 1.0f, 1.0f ));
            mEnemy->DisableAttackCollision();
        }
    });
    
    mStateMachine["Death"]->OnAnimFinished.Bind([this]()
    {
        if (mEnemy)
            mEnemy->Destroy();
    });

    auto& attackClip = mStateMachine["Attack"];
    attackClip->SetLoop(false);
    attackClip->SetAnimationSpeed(2.f);
    attackClip->mEvents[attackClip->GetEndFrame() * 0.3].Bind(std::bind(&AEnemy::EnableAttackCollision, mEnemy, 1.2f));
    attackClip->mEvents[attackClip->GetEndFrame() * 0.7].Bind(std::bind(&AEnemy::DisableAttackCollision, mEnemy));
    attackClip->OnAnimFinished.Bind([&]()
    {
        if (mEnemy)
        {
            // mEnemy->AddLocalLocation(attackClip->GetRMPosition());
            mEnemy->SetEnemyState(EEnemyState::Idle);
        }
    });

    AddAnimLink("Idle", "Walk", [&]() { return !mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
    AddAnimLink("Walk", "Idle", [&]() { return mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);

    AddAnimLink("Idle", "Death", [&]() { return mEnemy->mEnemyState == EEnemyState::Death; }, 0.5f);
    AddAnimLink("Walk", "Death", [&]() { return mEnemy->mEnemyState == EEnemyState::Death; }, 0.5f);
    AddAnimLink("Attack", "Death", [&]() { return mEnemy->mEnemyState == EEnemyState::Death; }, 0.5f);

    AddAnimLink("Idle", "Attack", [&]() { return mEnemy->mEnemyState == EEnemyState::Attack; }, 0.5f);
    AddAnimLink("Walk", "Attack", [&]() { return mEnemy->mEnemyState == EEnemyState::Attack; }, 0.5f);

    AddAnimLink("Attack", "Idle", [&]() { return mEnemy->mEnemyState == EEnemyState::Idle; }, 0.5f);
    // AddAnimLink("Attack", "Walk", [&](){return !mMovementComponent->GetVelocity().IsNearlyZero() && /*공격 끝난상태*/});


    SetState("Idle");
    mCurrentAnimation->Play();
}

void JKihyunAnimator::BeginPlay()
{
    JAnimator::BeginPlay();
}
