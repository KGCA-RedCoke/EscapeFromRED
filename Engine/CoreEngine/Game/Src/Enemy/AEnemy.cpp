#include "AEnemy.h"
#include "Core/Entity/Component/AI/BT_BOSS.h"
#include "Animator/JKihyunAnimator.h"
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

	mObjectFlags |= EObjectFlags::ShouldTick;

	mSkeletalMeshComponent = dynamic_cast<JSkeletalMeshComponent*>(GetChildComponentByType(
		 NAME_OBJECT_SKELETAL_MESH_COMPONENT));

	if (!mSkeletalMeshComponent)
	{
		mSkeletalMeshComponent = CreateDefaultSubObject<JSkeletalMeshComponent>("SkeletalMesh", this);
		mSkeletalMeshComponent->SetupAttachment(this);
	}

	if (!mBehaviorTree)
	{
		mBehaviorTree = CreateDefaultSubObject<BT_BOSS>("BehaviorTree", this);
	}


	
	APawn::Initialize();

	if (mSkeletalMeshComponent)
	{
		mAnimator = MakeUPtr<JKihyunAnimator>("Animator", mSkeletalMeshComponent);
		mAnimator->Initialize();
		mSkeletalMeshComponent->SetAnimator(mAnimator.get());
	}
	
	assert(mCollisionBox);
	mCollisionBox->OnComponentBeginOverlap.Bind(std::bind(&AEnemy::OnHit,
														  this,
														  std::placeholders::_1,
														  std::placeholders::_2));

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
	}
}

