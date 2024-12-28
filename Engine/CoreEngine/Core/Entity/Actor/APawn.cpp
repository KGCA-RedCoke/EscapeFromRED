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
    mCollisionBox = static_cast<JBoxComponent*>(GetChildComponentByType(NAME_COMPONENT_BOX));

    if (!mMovementComponent)
    {
        mMovementComponent = CreateDefaultSubObject<JPawnMovementComponent>("MovementComponent", this);
        mMovementComponent->Initialize();
    }
    if (!mLineComponent)
    {
        mLineComponent = CreateDefaultSubObject<JLineComponent>("LineComponent", this);
        mLineComponent->SetupAttachment(this);
        mLineComponent->Initialize();

        mLineComponent->OnComponentOverlap.Bind([this](ICollision* InOther, const FHitResult& HitResult)
        {
            auto* sceneComponent = dynamic_cast<JCollisionComponent*>(InOther);
            assert(sceneComponent);
            const ETraceType type = sceneComponent->GetTraceType();
            LOG_CORE_INFO("From :{}  To :{}, Hit Info(HitLocation.y) :{}", GetName(),
                          dynamic_cast<JSceneComponent*>(InOther)->GetName(), HitResult.HitLocation.y);

            switch (type)
            {
            case ETraceType::Pawn:
                {
                    mMaxHeight = FMath::Max(mMaxHeight, HitResult.HitLocation.y);
                    FVector currentLocation = GetWorldLocation();
                    
                    if (currentLocation.y > mMaxHeight)
                        currentLocation.y -= 98 * mDeltaTime;
                    SetWorldLocation(currentLocation);
                }
                break;
            case ETraceType::BlockingVolume:
                break;
            case ETraceType::Ground:
                {
                    mMaxHeight = FMath::Max(mMaxHeight, HitResult.HitLocation.y);
                    FVector currentLocation = GetWorldLocation();
                    mYVelocity += 980 * mDeltaTime;
                    if (currentLocation.y > mMaxHeight)
                        currentLocation.y -= mYVelocity * mDeltaTime;
                    else
                    {
                        currentLocation.y = mMaxHeight;
                        mYVelocity = 0.f;
                    }
                    SetWorldLocation(currentLocation);
                }
                break;
            }
        });
    }
    if (!mCollisionBox)
    {
        mCollisionBox = CreateDefaultSubObject<JBoxComponent>("CollisionBox", this);
        mCollisionBox->SetupAttachment(this);
        mCollisionBox->Initialize();

        mCollisionBox->OnComponentOverlap.Bind([this](ICollision* InOther, const FHitResult& HitResult)
        {
            auto* sceneComponent = dynamic_cast<JCollisionComponent*>(InOther);
            assert(sceneComponent);

            auto* Other = sceneComponent->GetOwnerActor();
            assert(Other);

            const ETraceType type = sceneComponent->GetTraceType();

            switch (type)
            {
            case ETraceType::Pawn:
                {
                    FVector ThisPosition = GetWorldLocation();
                    FVector OtherPosition = Other->GetWorldLocation();

                    // 두 물체의 상대적인 위치를 계산
                    FVector RelativePosition = ThisPosition - OtherPosition;

                    // 1. 침투 해결 (겹침 제거)
                    FVector correction = HitResult.HitNormal * HitResult.Distance * mDeltaTime;

                    // 물체가 밀려야 할 방향을 계산 (충돌 법선에 따라 위치 조정)
                    // 상대 위치의 방향에 따라 밀어내는 방향 결정
                    if (RelativePosition.Dot(HitResult.HitNormal) < 0)
                    {
                        // BoxA는 법선 반대 방향으로 이동, BoxB는 법선 방향으로 이동
                        ThisPosition -= correction; // BoxA를 법선 반대 방향으로 이동
                        OtherPosition += correction; // BoxB를 법선 방향으로 이동
                    }
                    else
                    {
                        // 반대로, BoxA는 법선 방향으로 이동, BoxB는 법선 반대 방향으로 이동
                        ThisPosition += correction; // BoxA를 법선 방향으로 이동
                        OtherPosition -= correction; // BoxB를 법선 반대 방향으로 이동
                    }
                    //
                    // LOG_CORE_INFO("correction: {},{},{}", correction.x, correction.y, correction.z);
                    // LOG_CORE_INFO("Before - ThisPosition: {},{},{}", ThisPosition.x, ThisPosition.y, ThisPosition.z);
                    // LOG_CORE_INFO("Before - OtherPosition: {},{},{}", OtherPosition.x, OtherPosition.y,
                    //               OtherPosition.z);

                    // 위치 이동 후, 물체들의 새로운 위치를 설정
                    SetWorldLocation(ThisPosition);
                    Other->SetWorldLocation(OtherPosition);

                    // LOG_CORE_INFO("After - ThisPosition: {},{},{}", ThisPosition.x, ThisPosition.y, ThisPosition.z);
                    // LOG_CORE_INFO("After - OtherPosition: {},{},{}", OtherPosition.x, OtherPosition.y, OtherPosition.z);
                }
                break;
            case ETraceType::BlockingVolume:
                {
                    FVector ThisPosition = GetWorldLocation();
                    FVector OtherPosition = Other->GetWorldLocation();
                    FVector RelativePosition = ThisPosition - OtherPosition;
                    
                    FVector correction = HitResult.HitNormal * HitResult.Distance * mDeltaTime * 5;

                    ThisPosition -= (RelativePosition.Dot(HitResult.HitNormal) < 0) ? correction : -correction;
                    // ThisPosition -= correction;
                    SetWorldLocation(ThisPosition);
                }
                break;
            case ETraceType::Ground:
                {
                    // mLineComponent
                }
                break;
            // case ETraceType::Projectile:
            //     
            //     break;
            }
        });
    }
}

void APawn::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);
    mDeltaTime = DeltaTime;
}

void APawn::Destroy()
{
    AActor::Destroy();
}

// void APawn::HandlePawnToPawn(const FHitResult& HitResult)
// {
//     FVector pushDirection = HitResult.HitNormal * HitResult.Distance * 5;
//     pushDirection *= mDeltaTime;
//     FVector ThisPosition = GetWorldLocation();
//     FVector OtherPosition = DstCollision.mOwnerActor->GetWorldLocation();
//     
//     mOwnerActor->SetWorldLocation(ThisPosition - pushDirection);
//     OtherBoxComp.mOwnerActor->SetWorldLocation(OtherPosition + pushDirection);
// }
