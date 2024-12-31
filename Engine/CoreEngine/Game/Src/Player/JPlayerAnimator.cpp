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

	AddAnimationClip("Idle_Free",
					 "Game/Animation/Player/FPP_Halb_Idle.jasset");
	AddAnimationClip("Walk_Free",
					 "Game/Animation/Player/FPP_Halb_Walk.jasset");
	AddAnimationClip("Run_Free",
					 "Game/Animation/Player/FPP_Halb_Run.jasset");
	AddAnimationClip("Combo1", "Game/Animation/Player/FPP_Halb_Attack_R.jasset");
	AddAnimationClip("Combo2", "Game/Animation/Player/FPP_Halb_Attack_LD.jasset");
	AddAnimationClip("Combo3", "Game/Animation/Player/FPP_Halb_Attack_RU.jasset");
	AddAnimationClip("Combo4", "Game/Animation/Player/FPP_Halb_AttackHeavy_D.jasset");

	auto* combo1Anim = mStateMachine["Combo1"].get();
	auto* combo2Anim = mStateMachine["Combo2"].get();
	auto* combo3Anim = mStateMachine["Combo3"].get();
	auto* combo4Anim = mStateMachine["Combo4"].get();
	combo1Anim->SetLoop(false);
	combo2Anim->SetLoop(false);
	combo3Anim->SetLoop(false);
	combo4Anim->SetLoop(false);
	combo1Anim->SetAnimationSpeed(1.5f);
	combo2Anim->SetAnimationSpeed(1.5f);
	combo3Anim->SetAnimationSpeed(1.5f);
	combo4Anim->SetAnimationSpeed(2.f);


	combo1Anim->mEvents[combo1Anim->GetEndFrame() * 0.27].
			Bind(std::bind(&APlayerCharacter::OnMeleeAttack, mOwnerCharacter));
	combo1Anim->mEvents[combo1Anim->GetEndFrame() * 0.4].Bind(std::bind(&APlayerCharacter::DisableMeleeCollision,
																		mOwnerCharacter));
	combo1Anim->mEvents[combo1Anim->GetEndFrame() * 0.7].Bind(std::bind(&APlayerCharacter::OnMeleeAttackFinished,
																		mOwnerCharacter));

	combo2Anim->mEvents[combo2Anim->GetEndFrame() * 0.24].
			Bind(std::bind(&APlayerCharacter::OnMeleeAttack, mOwnerCharacter));
	combo2Anim->mEvents[combo2Anim->GetEndFrame() * 0.53].Bind(std::bind(&APlayerCharacter::DisableMeleeCollision,
																		 mOwnerCharacter));
	combo2Anim->mEvents[combo2Anim->GetEndFrame() * 0.75].Bind(std::bind(&APlayerCharacter::OnMeleeAttackFinished,
																		 mOwnerCharacter));

	combo3Anim->mEvents[combo3Anim->GetEndFrame() * 0.25].
			Bind(std::bind(&APlayerCharacter::OnMeleeAttack, mOwnerCharacter));
	combo3Anim->mEvents[combo3Anim->GetEndFrame() * 0.47].Bind(std::bind(&APlayerCharacter::DisableMeleeCollision,
																		 mOwnerCharacter));
	combo3Anim->mEvents[combo3Anim->GetEndFrame() * 0.60].Bind(std::bind(&APlayerCharacter::OnMeleeAttackFinished,
																		 mOwnerCharacter));
	combo4Anim->mEvents[combo4Anim->GetEndFrame() * 0.47].
			Bind(std::bind(&APlayerCharacter::OnMeleeAttack, mOwnerCharacter));
	combo4Anim->mEvents[combo4Anim->GetEndFrame() * 0.67].Bind(std::bind(&APlayerCharacter::DisableMeleeCollision,
																		 mOwnerCharacter));
	combo4Anim->mEvents[combo4Anim->GetEndFrame() * 0.8].Bind(std::bind(&APlayerCharacter::OnMeleeAttackFinished,
																		mOwnerCharacter));

	AddAnimLink("Idle_Free", "Walk_Free", [&](){ return !mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
	AddAnimLink("Walk_Free", "Idle_Free", [&](){ return mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);
	AddAnimLink("Walk_Free", "Run_Free", [&](){ return mOwnerCharacter->bShouldRun; }, 0.2f);
	AddAnimLink("Run_Free", "Walk_Free", [&](){ return !mOwnerCharacter->bShouldRun; }, 0.2f);
	AddAnimLink("Run_Free", "Idle_Free", [&](){ return mMovementComponent->GetVelocity().IsNearlyZero(); }, 0.2f);

	AddAnimLink("Idle_Free", "Combo1", [&](){ return mOwnerCharacter->bShouldAttack; }, 0.2f);
	AddAnimLink("Walk_Free", "Combo1", [&](){ return mOwnerCharacter->bShouldAttack; }, 0.2f);
	AddAnimLink("Run_Free", "Combo1", [&](){ return mOwnerCharacter->bShouldAttack; }, 0.2f);

	AddAnimLink("Combo1",
				"Combo2",
				[&](){ return mOwnerCharacter->mAttackCombo == 1; },
				0.2f);
	AddAnimLink("Combo1",
				"Run_Free",
				[&](){
					return mCurrentAnimation->GetElapsedRatio() > 0.8 && !mMovementComponent->GetVelocity().IsNearlyZero()
							&&
							mOwnerCharacter->bShouldRun;
				},
				0.2f);
	AddAnimLink("Combo1",
				"Walk_Free",
				[&](){
					return mCurrentAnimation->GetElapsedRatio() > 0.8 && !mMovementComponent->GetVelocity().IsNearlyZero();
				},
				0.2f);
	AddAnimLink("Combo1",
				"Idle_Free",
				[&](){ return mCurrentAnimation->GetElapsedRatio() > 0.8; },
				0.2f);


	AddAnimLink("Combo2",
				"Combo3",
				[&](){ return mOwnerCharacter->mAttackCombo == 2; },
				0.2f);
	AddAnimLink("Combo2",
				"Run_Free",
				[&](){
					return mCurrentAnimation->GetElapsedRatio() > 0.8 && !mMovementComponent->GetVelocity().IsNearlyZero() &&
							mOwnerCharacter->bShouldRun;
				},
				0.2f);
	AddAnimLink("Combo2",
				"Walk_Free",
				[&](){ return mCurrentAnimation->GetElapsedRatio() > 0.8 && !mMovementComponent->GetVelocity().IsNearlyZero(); },
				0.2f);
	AddAnimLink("Combo2",
				"Idle_Free",
				[&](){ return mCurrentAnimation->GetElapsedRatio() > 0.8; },
				0.2f);

	AddAnimLink("Combo3",
				"Combo4",
				[&](){ return mOwnerCharacter->mAttackCombo == 3; },
				0.2f);
	AddAnimLink("Combo3",
				"Run_Free",
				[&](){
					return mCurrentAnimation->GetElapsedRatio() > 0.8 && !mMovementComponent->GetVelocity().IsNearlyZero() &&
							mOwnerCharacter->bShouldRun;
				},
				0.2f);
	AddAnimLink("Combo3",
				"Walk_Free",
				[&](){ return mCurrentAnimation->GetElapsedRatio() > 0.8 && !mMovementComponent->GetVelocity().IsNearlyZero(); },
				0.2f);
	AddAnimLink("Combo3",
				"Idle_Free",
				[&](){ return mCurrentAnimation->GetElapsedRatio() > 0.8; },
				0.2f);


	AddAnimLink("Combo4",
				"Run_Free",
				[&](){
					return mCurrentAnimation->GetElapsedRatio() > 0.8 && !mMovementComponent->GetVelocity().IsNearlyZero() &&
							mOwnerCharacter->bShouldRun;
				},
				0.2f);
	AddAnimLink("Combo4",
				"Walk_Free",
				[&](){ return mCurrentAnimation->GetElapsedRatio() > 0.8 && !mMovementComponent->GetVelocity().IsNearlyZero(); },
				0.2f);
	AddAnimLink("Combo4",
				"Idle_Free",
				[&](){ return mCurrentAnimation->GetElapsedRatio() > 0.8; },
				0.2f);


	SetState("Idle_Free");
	mCurrentAnimation->Play();
}

void JPlayerAnimator::BeginPlay()
{
	JAnimator::BeginPlay();
}
