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

        mLineComponent->OnComponentBeginOverlap.Bind([this](ICollision* InOther, const FHitResult& HitResult)
        {
            auto* sceneComponent = dynamic_cast<JCollisionComponent*>(InOther);
           assert(sceneComponent);
           const ETraceType type = sceneComponent->GetTraceType();
            LOG_CORE_INFO("From :{}  To :{}, Hit Info(HitLocation.y) :{}", GetName(), dynamic_cast<JSceneComponent*>(InOther)->GetName(), HitResult.HitLocation.y);

          switch (type)
          {
          case ETraceType::Pawn:
              break;
          case ETraceType::BlockingVolume:
              break;
          case ETraceType::Ground:
              {
                  mMaxHeight = FMath::Max(mMaxHeight, HitResult.HitLocation.y);
                  FVector currentLocation = GetWorldLocation();
                  currentLocation.y = mMaxHeight;
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

        mCollisionBox->OnComponentBeginOverlap.Bind([this](ICollision* InOther, const FHitResult& HitResult)
        {
            auto* sceneComponent = dynamic_cast<JCollisionComponent*>(InOther);
            assert(sceneComponent);
            const ETraceType type = sceneComponent->GetTraceType();
            
           switch (type) {
           case ETraceType::Pawn:
               {
                     
                   LOG_CORE_INFO("From :{}  To :{}, Hit Info(HitLocation) x :{}", GetName(), dynamic_cast<JSceneComponent*>(InOther)->GetName(), HitResult.HitLocation.x);
                   FVector pushDirection = HitResult.HitNormal * HitResult.Distance * 5;
                   pushDirection *= mDeltaTime;
                   FVector ThisPosition = GetWorldLocation();
                   FVector OtherPosition = sceneComponent->GetWorldLocation();
                   
                   SetWorldLocation(ThisPosition - pushDirection);
                   sceneComponent->SetWorldLocation(OtherPosition + pushDirection);
                   
               }
               break;
           case ETraceType::BlockingVolume:
               
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
