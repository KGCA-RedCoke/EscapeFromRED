#include "APawn.h"

#include "Core/Entity/Component/Movement/JPawnMovementComponent.h"
#define INIT_HEIGHT 0.f

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

	mMovementComponent = static_cast<JPawnMovementComponent*>(GetChildComponentByType(NAME_COMPONENT_PAWN_MOVEMENT));
	mLineComponent     = static_cast<JLineComponent*>(GetChildComponentByType(NAME_COMPONENT_RAY));
	mCollisionSphere   = static_cast<JSphereComponent*>(GetChildComponentByType(NAME_COMPONENT_SPHERE));

	if (!mMovementComponent)
	{
		mMovementComponent = CreateDefaultSubObject<JPawnMovementComponent>("MovementComponent", this);
	}
	if (!mLineComponent)
	{
		mLineComponent = CreateDefaultSubObject<JLineComponent>("LineComponent", this);
		mLineComponent->SetupAttachment(this);

		mLineComponent->OnComponentOverlap.Bind([this](ICollision* InOther, const FHitResult& OutHitResult){
			auto* sceneComponent = dynamic_cast<JCollisionComponent*>(InOther);
			assert(sceneComponent);
			const ETraceType type = sceneComponent->GetTraceType();

			switch (type)
			{
			case ETraceType::Pawn:
				break;
			case ETraceType::BlockingVolume:
				break;
			case ETraceType::Ground:
				{
					float MaxHeight = FMath::Max(mMaxHeight, OutHitResult.HitLocation.y);

					FVector currentLocation = GetLocalLocation();
					mYVelocity += 980 * mDeltaTime;
					// mLocalLocation.y = MaxHeight;	
					if (currentLocation.y > MaxHeight + FLT_EPSILON)
					{
						AddLocalLocation(FVector(0, -mYVelocity * mDeltaTime, 0));
					}
					else if (currentLocation.y < MaxHeight - FLT_EPSILON)
					{
						// mLocalLocation.y = MaxHeight - FLT_EPSILON;
						AddLocalLocation(FVector(0, MaxHeight - currentLocation.y, 0));
						mYVelocity = 0.f;
					}
					mMaxHeight = MaxHeight;
				}
				break;
			}
		});


	}
	if (!mCollisionSphere)
	{
		mCollisionSphere = CreateDefaultSubObject<JSphereComponent>("CollisionSphere", this);
		mCollisionSphere->SetupAttachment(this);
		mCollisionSphere->SetLocalLocation({0, 115, 0});
		mCollisionSphere->SetLocalScale({1, 2, 1});

		mCollisionSphere->OnComponentOverlap.Bind([&](ICollision* InOther, const FHitResult& HitResult){

			auto* sceneComponent = dynamic_cast<JCollisionComponent*>(InOther);
			assert(sceneComponent);

			auto* Other = sceneComponent->GetParentSceneComponent();
			assert(Other);

			const ETraceType type = sceneComponent->GetTraceType();

			switch (type)
			{
			case ETraceType::Pawn:
				{
					FVector RelativePosition = mWorldLocation - Other->GetWorldLocation();

					FVector correction = HitResult.HitNormal * HitResult.Distance * 0.5;

					correction = (RelativePosition.Dot(HitResult.HitNormal) < 0) ? -correction : correction;
					AddLocalLocation(correction);
				}
				break;
			case ETraceType::BlockingVolume:
				{
					FVector RelativePosition = mWorldLocation - Other->GetWorldLocation();

					FVector correction = HitResult.HitNormal * HitResult.Distance;

					correction = (RelativePosition.Dot(HitResult.HitNormal) < 0) ? -correction : correction;
					AddLocalLocation(correction);
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

	mLineComponent->SetLength(2000);

	AActor::Initialize();

}

void APawn::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);
	mDeltaTime = DeltaTime;
	mMaxHeight = INIT_HEIGHT;
}

void APawn::Destroy()
{
	AActor::Destroy();

	mLineComponent->Destroy();
	mCollisionSphere->Destroy();
}
