#include "JPlayerAnimator.h"

#include "APlayerCharacter.h"
#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"
#include "Core/Interface/JWorld.h"

JPlayerAnimator::JPlayerAnimator() {}

JPlayerAnimator::JPlayerAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp)
	: JAnimator(InName, InSkeletalComp)
{
	mOwnerCharacter = static_cast<APlayerCharacter*>(InSkeletalComp->GetOwnerActor());
}

JPlayerAnimator::JPlayerAnimator(JTextView InName, AActor* InOwnerActor)
	: JAnimator(InName) {}

void JPlayerAnimator::Initialize()
{
	JAnimator::Initialize();

	auto* attackAnim = GetWorld.AnimationManager->Load("Game/Animation/FPP_Halb_Attack_D.jasset",
													   mSkeletalMeshComponent);
	attackAnim->OnAnimStart.Bind([this](){
		mOwnerCharacter->mWeaponCollision->EnableCollision(true);
	});
	attackAnim->OnAnimBlendOut.Bind([this](){
		mOwnerCharacter->bShouldAttack = false;
		mOwnerCharacter->mWeaponCollision->EnableCollision(false);
	});
	attackAnim->OnAnimFinished.Bind([this](){
		mOwnerCharacter->bShouldAttack = false;
		mOwnerCharacter->mWeaponCollision->EnableCollision(false);
	});

	AddAnimationClip("Idle_Free",
					 "Game/Animation/FPP_Halb_Idle.jasset");
	AddAnimationClip("Walk_Free",
					 "Game/Animation/FPP_Halb_Walk.jasset");
	AddAnimationClip("Run_Free",
					 "Game/Animation/FPP_Halb_Run.jasset");
	AddAnimationClip("Attack", attackAnim);

	AddAnimLink("Idle_Free", "Walk_Free", [&](){ return !mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
	AddAnimLink("Walk_Free", "Idle_Free", [&](){ return mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
	AddAnimLink("Walk_Free", "Run_Free", [&](){ return mOwnerCharacter->bShouldRun; }, 0.2f);
	AddAnimLink("Run_Free", "Walk_Free", [&](){ return !mOwnerCharacter->bShouldRun; }, 0.2f);
	AddAnimLink("Run_Free", "Idle_Free", [&](){ return mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);

	AddAnimLink("Idle_Free", "Attack", [&](){ return mOwnerCharacter->bShouldAttack; }, 0.2f);
	AddAnimLink("Walk_Free", "Attack", [&](){ return mOwnerCharacter->bShouldAttack; }, 0.2f);
	AddAnimLink("Run_Free", "Attack", [&](){ return mOwnerCharacter->bShouldAttack; }, 0.2f);

	AddAnimLink("Attack", "Idle_Free", [&](){ return !mOwnerCharacter->bShouldAttack; }, 0.2f);

	SetState("Idle_Free");
	mCurrentAnimation->Play();
}

void JPlayerAnimator::BeginPlay()
{
	JAnimator::BeginPlay();
}
