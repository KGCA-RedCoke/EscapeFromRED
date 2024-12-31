#include "ABoss.h"
#include "Core/Entity/Component/AI/BT_BOSS.h"
#include "Animator/JKillerClownAnimator.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"

ABoss::ABoss()
	: APawn(),
	  mSkeletalMeshComponent(nullptr)
{
	mObjectType = NAME_OBJECT_ENEMY;
}

ABoss::ABoss(JTextView InName)
	: APawn(InName),
	  mSkeletalMeshComponent(nullptr)
{
	mObjectType = NAME_OBJECT_ENEMY;
}

void ABoss::Initialize()
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
		mAnimator = MakeUPtr<JKillerClownAnimator>("Animator", mSkeletalMeshComponent);
		mAnimator->Initialize();
		mSkeletalMeshComponent->SetAnimator(mAnimator.get());
	}

	assert(mCollisionSphere);
	mCollisionSphere->OnComponentBeginOverlap.Bind(std::bind(&ABoss::OnHit,
															 this,
															 std::placeholders::_1,
															 std::placeholders::_2));

}

void ABoss::Tick(float DeltaTime)
{
	APawn::Tick(DeltaTime);
}

void ABoss::Destroy()
{
	APawn::Destroy();
}

uint32_t ABoss::GetType() const
{
	return HASH_ASSET_TYPE_Enemy;
}

bool ABoss::Serialize_Implement(std::ofstream& FileStream)
{
	if (!APawn::Serialize_Implement(FileStream))
	{
		return false;
	}

	// Enemy Type
	Utils::Serialization::Serialize_Primitive(&mBossType, sizeof(mBossType), FileStream);


	return true;
}

bool ABoss::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!APawn::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	// Enemy Type
	// Utils::Serialization::DeSerialize_Primitive(&mBossType, sizeof(mBossType), InFileStream);

	return true;
}

void ABoss::ShowEditor()
{
	APawn::ShowEditor();

	ImGui::TextColored(ImColor(1, 0, 0, 1),u8("몬스터 타입"));

	if (ImGui::BeginCombo("##Enemy Type", GetEnemyTypeString(mBossType)))
	{
		for (uint8_t i = 0; i < EnumAsByte(EBossType::MAX); ++i)
		{
			if (ImGui::Selectable(GetEnemyTypeString(static_cast<EBossType>(i))))
			{
				mBossType = static_cast<EBossType>(i);
				break;
			}
		}
		ImGui::EndCombo();
	}
}

void ABoss::OnHit(ICollision* InActor, const FHitResult& HitResult)
{
	const ETraceType traceType = InActor->GetTraceType();
	if (traceType == ETraceType::PlayerWeapon)
	{
		mBossState = EBossState::Death;
		mCollisionSphere->Destroy();
	}
}
