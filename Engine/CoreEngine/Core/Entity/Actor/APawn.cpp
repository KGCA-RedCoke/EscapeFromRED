#include "APawn.h"

#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"

APawn::APawn()
    : mMovementComponent(nullptr)
{
    mObjectType = NAME_OBJECT_PAWN;
}

APawn::APawn(JTextView Name)
    : AActor(Name)
{
    mObjectType = NAME_OBJECT_PAWN;
}

APawn::~APawn() = default;

uint32_t APawn::GetType() const
{
    return HASH_ASSET_TYPE_Pawn;
}

bool APawn::Serialize_Implement(std::ofstream& FileStream)
{
    if (!AActor::Serialize_Implement(FileStream))
    {
        return false;
    }

    return true;
}

bool APawn::DeSerialize_Implement(std::ifstream& InFileStream)
{
    if (!AActor::DeSerialize_Implement(InFileStream))
    {
        return false;
    }


    return true;
}

void APawn::Initialize()
{
    AActor::Initialize();

    mMovementComponent = static_cast<JPawnMovementComponent*>(GetChildComponentByType(NAME_COMPONENT_PAWN_MOVEMENT));
    mLineComponent = static_cast<JLineComponent*>(GetChildComponentByType(NAME_COMPONENT_RAY));

    if (!mMovementComponent)
    {
        mMovementComponent = CreateDefaultSubObject<JPawnMovementComponent>("MovementComponent", this);
    }
    if (!mLineComponent)
    {
        mLineComponent = CreateDefaultSubObject<JLineComponent>("LineComponent", this);
        mLineComponent->SetupAttachment(this);
    }
}

void APawn::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);
}

void APawn::Destroy()
{
    AActor::Destroy();
}
