#include "JPlayerAnimator.h"

#include "APlayerCharacter.h"
#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Audio/MSoundManager.h"
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

	mWalkLSound = GetWorld.SoundManager->Load("rsc/GameResource/Sound/W_Dirt_1.wav");
	mWalkRSound = GetWorld.SoundManager->Load("rsc/GameResource/Sound/W_Dirt_4.wav");
	
	mRunLSound = GetWorld.SoundManager->Load("rsc/GameResource/Sound/W_Dirt_4.wav");
	mRunRSound = GetWorld.SoundManager->Load("rsc/GameResource/Sound/W_Dirt_4.wav");
	
	mAttackBasicSound = GetWorld.SoundManager->Load("rsc/GameResource/Sound/AttackEnemy.mp3");
	mAttackEnemySound = GetWorld.SoundManager->Load("rsc/GameResource/Sound/AttackEnemy.mp3");

	AddAnimationClip("Idle_Free",
	                 "Game/Animation/Player/FPP_Halb_Idle.jasset", true);
	AddAnimationClip("Walk_Free",
	                 "Game/Animation/Player/FPP_Halb_Walk.jasset", true);
	AddAnimationClip("Run_Free",
	                 "Game/Animation/Player/FPP_Halb_Run.jasset", true);
	AddAnimationClip("Combo1", "Game/Animation/Player/FPP_Halb_Attack_R.jasset");
	AddAnimationClip("Combo2", "Game/Animation/Player/FPP_Halb_Attack_LD.jasset");
	AddAnimationClip("Combo3", "Game/Animation/Player/FPP_Halb_Attack_RU.jasset");
	AddAnimationClip("Combo4", "Game/Animation/Player/FPP_Halb_AttackHeavy_D.jasset");

	// Combo Animation Event Binding
	BindEvents();

	// ---------------------------------- Idle ----------------------------------
	AddAnimLink("Idle_Free", "Walk_Free", [&](){ return bIsMoving; }, 0.2f);
	AddAnimLink("Idle_Free", "Combo1", [&](){ return mOwnerCharacter->bShouldAttack; }, 0.2f);

	// ---------------------------------- Walk ----------------------------------
	AddAnimLink("Walk_Free", "Idle_Free", [&](){ return !bIsMoving; }, 0.2f);
	AddAnimLink("Walk_Free", "Combo1", [&](){ return mOwnerCharacter->bShouldAttack; }, 0.2f);
	AddAnimLink("Walk_Free", "Run_Free", [&](){ return bIsSprinting; }, 0.2f);

	// ---------------------------------- Run ----------------------------------
	AddAnimLink("Run_Free", "Walk_Free", [&](){ return !bIsSprinting; }, 0.2f);
	AddAnimLink("Run_Free", "Combo1", [&](){ return mOwnerCharacter->bShouldAttack; }, 0.2f);
	AddAnimLink("Run_Free", "Idle_Free", [&](){ return !bIsMoving; }, 0.2f);


	// ---------------------------------- Combo1 ----------------------------------
	AddAnimLink("Combo1", "Combo2", [&](){ return mOwnerCharacter->mAttackCombo == 1; }, 0.2f);
	AddAnimLink("Combo1",
				"Run_Free",
				[&](){ return GetAnimElapsedRatio() > 0.8 && bIsMoving && bIsSprinting; },
				0.2f);
	AddAnimLink("Combo1",
				"Walk_Free",
				[&](){
					return GetAnimElapsedRatio() > 0.8 && bIsMoving;
				},
				0.2f);
	AddAnimLink("Combo1",
				"Idle_Free",
				[&](){ return GetAnimElapsedRatio() > 0.8; },
				0.2f);

	// ---------------------------------- Combo2 ----------------------------------
	AddAnimLink("Combo2",
				"Combo3",
				[&](){ return mOwnerCharacter->mAttackCombo == 2; },
				0.2f);
	AddAnimLink("Combo2",
				"Run_Free",
				[&](){
					return GetAnimElapsedRatio() > 0.8 && bIsMoving
							&&
							bIsSprinting;
				},
				0.2f);
	AddAnimLink("Combo2",
				"Walk_Free",
				[&](){
					return GetAnimElapsedRatio() > 0.8 && bIsMoving;
				},
				0.2f);
	AddAnimLink("Combo2",
				"Idle_Free",
				[&](){ return GetAnimElapsedRatio() > 0.8; },
				0.2f);

	// ---------------------------------- Combo3 ----------------------------------
	AddAnimLink("Combo3",
				"Combo4",
				[&](){ return mOwnerCharacter->mAttackCombo == 3; },
				0.2f);
	AddAnimLink("Combo3",
				"Run_Free",
				[&](){
					return GetAnimElapsedRatio() > 0.8 && bIsMoving
							&&
							bIsSprinting;
				},
				0.2f);
	AddAnimLink("Combo3",
				"Walk_Free",
				[&](){
					return GetAnimElapsedRatio() > 0.8 && bIsMoving;
				},
				0.2f);
	AddAnimLink("Combo3",
				"Idle_Free",
				[&](){ return GetAnimElapsedRatio() > 0.8; },
				0.2f);

	// ---------------------------------- Combo4 ----------------------------------
	AddAnimLink("Combo4",
				"Run_Free",
				[&](){
					return GetAnimElapsedRatio() > 0.8 && bIsMoving
							&&
							bIsSprinting;
				},
				0.2f);
	AddAnimLink("Combo4",
				"Walk_Free",
				[&](){
					return GetAnimElapsedRatio() > 0.8 && bIsMoving;
				},
				0.2f);
	AddAnimLink("Combo4",
				"Idle_Free",
				[&](){ return GetAnimElapsedRatio() > 0.8; },
				0.2f);


	SetState("Idle_Free");
	mCurrentAnimation->Play();
}

void JPlayerAnimator::BindEvents()
{
	auto* walkAnim = mStateMachine["Walk_Free"].get();
	auto* runAnim = mStateMachine["Run_Free"].get();	
	
	auto* combo1Anim = mStateMachine["Combo1"].get();
	auto* combo2Anim = mStateMachine["Combo2"].get();
	auto* combo3Anim = mStateMachine["Combo3"].get();
	auto* combo4Anim = mStateMachine["Combo4"].get();
	combo1Anim->SetLoop(false);
	combo2Anim->SetLoop(false);
	combo3Anim->SetLoop(false);
	combo4Anim->SetLoop(false);
	combo1Anim->SetAnimationSpeed(1.5f);
	combo2Anim->SetAnimationSpeed(1.8f);
	combo3Anim->SetAnimationSpeed(1.5f);
	combo4Anim->SetAnimationSpeed(2.f);


	combo1Anim->mEvents[combo1Anim->GetEndFrame() * 0.2].
			Bind(std::bind(&APlayerCharacter::OnMeleeAttack, mOwnerCharacter));
	combo1Anim->mEvents[combo1Anim->GetEndFrame() * 0.5].Bind(std::bind(&APlayerCharacter::DisableMeleeCollision,
																		mOwnerCharacter));
	combo1Anim->mEvents[combo1Anim->GetEndFrame() * 0.75].Bind(std::bind(&APlayerCharacter::OnMeleeAttackFinished,
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

	const uint32_t walkEndFrame = walkAnim->GetEndFrame();
	const uint32_t runEndFrame = runAnim->GetEndFrame();
	
	for (uint32_t i = 0; i < walkEndFrame; i+=14)
	{
		walkAnim->mEvents[i].Bind([&,i]()  
		{
			if (i / 14 % 2 == 0) 
			{
				mWalkLSound->Play();
			}
			else 
			{
				mWalkRSound->Play();
			}
		});
	}
	
	for (uint32_t i = 0; i < runEndFrame; i+=10)
	{
		runAnim->mEvents[i].Bind([&]()
		{
			mRunLSound->Play();
		});
	}

	
	combo1Anim->mEvents[combo1Anim->GetEndFrame()*0.1].Bind([&]()
	{
		mAttackBasicSound->Play();
	});
	
	combo2Anim->mEvents[combo2Anim->GetEndFrame()*0.2].Bind([&]()
	{
		mAttackBasicSound->Play();
	});
	
	combo3Anim->mEvents[combo3Anim->GetEndFrame()*0.2].Bind([&]()
	{
		mAttackBasicSound->Play();
	});
	
	combo4Anim->mEvents[combo4Anim->GetEndFrame()*0.4].Bind([&]()
	{
		mAttackBasicSound->Play();
	});
	
	

	
	

	
}

void JPlayerAnimator::BeginPlay()
{
	JAnimator::BeginPlay();
}

void JPlayerAnimator::Tick(float DeltaTime)
{
	bIsMoving    = !mMovementComponent->GetVelocity().IsNearlyZero();
	bIsSprinting = mOwnerCharacter->bShouldRun;

	JAnimator::Tick(DeltaTime);
}
