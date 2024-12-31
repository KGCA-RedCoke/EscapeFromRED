#include "AKillerClown.h"
#include "Core/Entity/Component/AI/BT_BOSS.h"
#include "Animator/JKillerClownAnimator.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Entity/Component/Mesh/JStaticMeshComponent.h"

AKillerClown::AKillerClown()
    : AEnemy()
{
    mObjectType = NAME_OBJECT_KILLERCLOWN;
    mEnemyType = EEnemyType::Clown;
}

AKillerClown::AKillerClown(JTextView InName)
    : AEnemy(InName)
{
    mObjectType = NAME_OBJECT_KILLERCLOWN;
    mEnemyType = EEnemyType::Clown;
}

void AKillerClown::Initialize()
{
    mSkeletalMeshComponent = dynamic_cast<JSkeletalMeshComponent*>(GetChildSceneComponentByType(
            NAME_OBJECT_SKELETAL_MESH_COMPONENT));
    if (!mSkeletalMeshComponent)
    {
        mSkeletalMeshComponent = CreateDefaultSubObject<JSkeletalMeshComponent>("SkeletalMesh", this);
        mSkeletalMeshComponent->SetupAttachment(this);
    }

    mHammerMesh = dynamic_cast<JStaticMeshComponent*>( GetChildSceneComponentByType(NAME_OBJECT_STATIC_MESH_COMPONENT));
    if (!mHammerMesh)
    {
        mHammerMesh = CreateDefaultSubObject<JStaticMeshComponent>("HammerMesh", this);
        mHammerMesh->SetMeshObject("Game/Mesh/SM_KC_Hammer.jasset");
        mHammerMesh->SetupAttachment(mSkeletalMeshComponent);
    }
    mHammerMesh->AttachToBoneSocket(mSkeletalMeshComponent, "Hammer_M");
    mHammerMesh->SetLocalRotation({0, 90, 0});

    if (!mBehaviorTree)
    {
        mBehaviorTree = CreateDefaultSubObject<BT_BOSS>("BehaviorTree", this);
    }
    APawn::Initialize();

    if (mSkeletalMeshComponent && mSkeletalMeshComponent->GetSkeletalMesh())
    {
        mAnimator = MakeUPtr<JKillerClownAnimator>("Animator", mSkeletalMeshComponent);
        mAnimator->Initialize();
        mSkeletalMeshComponent->SetAnimator(mAnimator.get());
    }

    assert(mCollisionSphere);
    mCollisionSphere->OnComponentBeginOverlap.Bind(std::bind(&AKillerClown::OnHit,
                                                             this,
                                                             std::placeholders::_1,
                                                             std::placeholders::_2));
}

void AKillerClown::Tick(float DeltaTime)
{
    APawn::Tick(DeltaTime);
}

void AKillerClown::Destroy()
{
    APawn::Destroy();
}

uint32_t AKillerClown::GetType() const
{
    return HASH_ASSET_TYPE_KillerClown;
}

bool AKillerClown::Serialize_Implement(std::ofstream& FileStream)
{
    if (!APawn::Serialize_Implement(FileStream))
    {
        return false;
    }

    // Enemy Type
    // Utils::Serialization::Serialize_Primitive(&mBossType, sizeof(mBossType), FileStream);


    return true;
}

bool AKillerClown::DeSerialize_Implement(std::ifstream& InFileStream)
{
    if (!APawn::DeSerialize_Implement(InFileStream))
    {
        return false;
    }

    // Enemy Type
    // Utils::Serialization::DeSerialize_Primitive(&mBossType, sizeof(mBossType), InFileStream);

    return true;
}

void AKillerClown::ShowEditor()
{
    APawn::ShowEditor();
    //
    // ImGui::TextColored(ImColor(1, 0, 0, 1),u8("몬스터 타입"));
    //
    // if (ImGui::BeginCombo("##Enemy Type", GetEnemyTypeString(mBossType)))
    // {
    // 	for (uint8_t i = 0; i < EnumAsByte(EBossType::MAX); ++i)
    // 	{
    // 		if (ImGui::Selectable(GetEnemyTypeString(static_cast<EBossType>(i))))
    // 		{
    // 			mBossType = static_cast<EBossType>(i);
    // 			break;
    // 		}
    // 	}
    // 	ImGui::EndCombo();
    // }
}

void AKillerClown::OnHit(ICollision* InActor, const FHitResult& HitResult)
{
    const ETraceType traceType = InActor->GetTraceType();
    if (traceType == ETraceType::PlayerWeapon)
    {
        mBossState = EBossState::Death;
        mCollisionSphere->Destroy();
    }
}
