#include "JKihyunAnimator.h"

#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"
#include "Core/Interface/JWorld.h"
#include "Game/Src/Enemy/AEnemy.h"

JKihyunAnimator::JKihyunAnimator()
{}

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
					 "Game/Animation/BigZombie/BZ_Idle.jasset");
	AddAnimationClip("Walk",
					 "Game/Animation/BigZombie/BZ_Run.jasset");
	AddAnimationClip("Death",
					 "Game/Animation/BigZombie/BZ_Death_Spinning.jasset");

	mStateMachine["Death"]->OnAnimFinished.Bind([this](){
		if (mEnemy)
			mEnemy->Destroy();
	});

	AddAnimLink("Idle", "Walk", [&](){ return !mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
	AddAnimLink("Walk", "Idle", [&](){ return mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);

	AddAnimLink("Idle", "Death", [&](){ return mEnemy->mEnemyState == EEnemyState::Death; }, 0.5f);
	AddAnimLink("Walk", "Death", [&](){ return mEnemy->mEnemyState == EEnemyState::Death; }, 0.5f);


	SetState("Idle");
	mCurrentAnimation->Play();
}

void JKihyunAnimator::BeginPlay()
{
	JAnimator::BeginPlay();
}
