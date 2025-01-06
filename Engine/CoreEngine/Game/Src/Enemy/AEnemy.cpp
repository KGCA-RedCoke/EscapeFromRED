#include "AEnemy.h"

#include "Animator/JButcherAnimator.h"
#include "Animator/JGirlAnimator.h"
#include "Animator/JKihyunAnimator.h"
#include "Animator/JPigAnimator.h"
#include "Core/Entity/Component/AI/BT_BigZombie.h"
#include "Core/Entity/Component/AI/BT_Butcher.h"
#include "Core/Entity/Component/AI/BT_Pig.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"

AEnemy::AEnemy()
	: APawn(),
	  mSkeletalMeshComponent(nullptr)
{
	mObjectType = NAME_OBJECT_ENEMY;
}

AEnemy::AEnemy(JTextView InName)
	: APawn(InName),
	  mSkeletalMeshComponent(nullptr)
{
	mObjectType = NAME_OBJECT_ENEMY;
}

void AEnemy::Initialize()
{
	mChildActorComponentIndices.clear();
	mActorComponents.clear();
	
	mObjectFlags |= EObjectFlags::ShouldTick;

	mSkeletalMeshComponent = dynamic_cast<JSkeletalMeshComponent*>(GetChildComponentByType(
		 NAME_OBJECT_SKELETAL_MESH_COMPONENT));
	if (!mSkeletalMeshComponent)
	{
		mSkeletalMeshComponent = CreateDefaultSubObject<JSkeletalMeshComponent>("SkeletalMesh", this);
		mSkeletalMeshComponent->SetupAttachment(this);
	}

	if (mSkeletalMeshComponent && mSkeletalMeshComponent->GetSkeletalMesh())
	{
		switch (mEnemyType)
		{
		case EEnemyType::Kihyun:
			mAnimator = MakeUPtr<JKihyunAnimator>("Animator", mSkeletalMeshComponent);
			mBehaviorTree = CreateDefaultSubObject<BT_BigZombie>("BehaviorTree", this);
			break;
		case EEnemyType::Girl:
			mAnimator = MakeUPtr<JGirlAnimator>("Animator", mSkeletalMeshComponent);
			mBehaviorTree = CreateDefaultSubObject<BT_BigZombie>("BehaviorTree", this);
			break;
		case EEnemyType::Clown:
			break;
		case EEnemyType::Pig:
			mAnimator = MakeUPtr<JPigAnimator>("Animator", mSkeletalMeshComponent);
			mBehaviorTree = CreateDefaultSubObject<BT_Pig>("BehaviorTree", this);
			break;
		case EEnemyType::Butcher:
			mAnimator = MakeUPtr<JButcherAnimator>("Animator", mSkeletalMeshComponent);
			mBehaviorTree = CreateDefaultSubObject<BT_Butcher>("BehaviorTree", this);
			break;
		case EEnemyType::MAX:
			break;
		}
		APawn::Initialize();
		mAnimator->Initialize();
		mSkeletalMeshComponent->SetAnimator(mAnimator.get());
	}
	else
	{
		APawn::Initialize();
	}

	assert(mCollisionSphere);
	mCollisionSphere->OnComponentBeginOverlap.Bind(std::bind(&AEnemy::OnHit,
															 this,
															 std::placeholders::_1,
															 std::placeholders::_2));
	mCollisionSphere->OnComponentEndOverlap.Bind(std::bind(&AEnemy::OnOut,
														   this,
														   std::placeholders::_1,
														   std::placeholders::_2));


	// 상호작용 가능한 Enemy Type (Pig, Butcher)
	mInteractionSphere = dynamic_cast<JSphereComponent*>(GetChildSceneComponentByName("Interaction"));
	if (mInteractionSphere)
	{
		mInteractionSphere->OnComponentBeginOverlap.Bind([&](ICollision* InOther, const FHitResult& HitResult){
			if (InOther->GetActorID() == StringHash("Player"))
				OnInteractionStart.Execute();
		});
		mInteractionSphere->OnComponentEndOverlap.Bind([&](ICollision* InOther, const FHitResult& HitResult){
			if (InOther->GetActorID() == StringHash("Player"))
			{
				OnInteractionEnd.Execute();
			}
		});
	}
}

void AEnemy::Tick(float DeltaTime)
{
	APawn::Tick(DeltaTime);
}

void AEnemy::Destroy()
{
	APawn::Destroy();
}

uint32_t AEnemy::GetType() const
{
	return HASH_ASSET_TYPE_Enemy;
}

bool AEnemy::Serialize_Implement(std::ofstream& FileStream)
{
	if (!APawn::Serialize_Implement(FileStream))
	{
		return false;
	}

	// Enemy Type
	Utils::Serialization::Serialize_Primitive(&mEnemyType, sizeof(mEnemyType), FileStream);


	return true;
}

bool AEnemy::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!APawn::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	// Enemy Type
	Utils::Serialization::DeSerialize_Primitive(&mEnemyType, sizeof(mEnemyType), InFileStream);

	return true;
}

void AEnemy::ShowEditor()
{
	APawn::ShowEditor();

	ImGui::TextColored(ImColor(1, 0, 0, 1),u8("몬스터 타입"));

	if (ImGui::BeginCombo("##Enemy Type", GetEnemyTypeString(mEnemyType)))
	{
		for (uint8_t i = 0; i < EnumAsByte(EEnemyType::MAX); ++i)
		{
			if (ImGui::Selectable(GetEnemyTypeString(static_cast<EEnemyType>(i))))
			{
				mEnemyType = static_cast<EEnemyType>(i);
				break;
			}
		}
		ImGui::EndCombo();
	}
}

void AEnemy::OnHit(ICollision* InActor, const FHitResult& HitResult)
{
	const ETraceType traceType = InActor->GetTraceType();
	if (traceType == ETraceType::PlayerWeapon)
	{
		mEnemyState = EEnemyState::Death;
		DisableAttackCollision();
		mCollisionSphere->Destroy();

	}

	OnEnemyHit.Execute(HitResult);
}

void AEnemy::OnOut(ICollision* InActor, const FHitResult& HitResult)
{
	OnEnemyOut.Execute();
}

void AEnemy::EnableAttackCollision(float radius)
{
	// mWeaponCollider->EnableCollision(true);
	// mWeaponCollider->SetLocalScale(FVector(radius, radius, radius));
}

void AEnemy::DisableAttackCollision()
{
	// mWeaponCollider->SetLocalScale(FVector(1.0f, 1.0f, 1.0f));
	// mWeaponCollider->EnableCollision(false);
}
