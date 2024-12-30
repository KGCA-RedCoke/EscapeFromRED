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
	mCollisionBox      = static_cast<JBoxComponent*>(GetChildComponentByType(NAME_COMPONENT_BOX));

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
	if (!mCollisionBox)
	{
		mCollisionBox = CreateDefaultSubObject<JBoxComponent>("CollisionBox", this);
		mCollisionBox->SetupAttachment(this);
		mCollisionBox->SetLocalLocation({0, 115, 0});
		mCollisionBox->SetLocalScale({1, 2, 1});

		mCollisionBox->OnComponentOverlap.Bind([&](ICollision* InOther, const FHitResult& HitResult){

			auto* sceneComponent = dynamic_cast<JCollisionComponent*>(InOther);
			assert(sceneComponent);

			auto* Other = sceneComponent->GetParentSceneComponent();
			assert(Other);

			const ETraceType type = sceneComponent->GetTraceType();

			switch (type)
			{
			case ETraceType::Pawn:
				{
					// 두 물체의 상대적인 위치를 계산
					FVector RelativePosition = GetWorldLocation() - Other->GetWorldLocation();

					// 1. 침투 해결 (겹침 제거)
					FVector correction = HitResult.HitNormal * HitResult.Distance * mDeltaTime * 5.f;

					// 물체가 밀려야 할 방향을 계산 (충돌 법선에 따라 위치 조정)
					// 상대 위치의 방향에 따라 밀어내는 방향 결정
					if (RelativePosition.Dot(HitResult.HitNormal) < 0)
					{
						// BoxA는 법선 반대 방향으로 이동, BoxB는 법선 방향으로 이동
						AddLocalLocation(-correction); // BoxA를 법선 반대 방향으로 이동
						Other->AddLocalLocation(correction); // BoxB를 법선 방향으로 이동
					}
					else
					{
						// 반대로, BoxA는 법선 방향으로 이동, BoxB는 법선 반대 방향으로 이동
						AddLocalLocation(correction); // BoxA를 법선 방향으로 이동
						Other->AddLocalLocation(-correction); // BoxB를 법선 반대 방향으로 이동
					}
				}
				break;
			case ETraceType::BlockingVolume:
				{
					FVector ThisPosition     = GetWorldLocation();
					FVector RelativePosition = mWorldLocation - Other->GetWorldLocation();

					FVector correction = HitResult.HitNormal * HitResult.Distance * mDeltaTime * 5.f;

					ThisPosition -= (RelativePosition.Dot(HitResult.HitNormal) < 0) ? correction : -correction;
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
		mCollisionBox->OnComponentOverlap.Bind([&](ICollision* InOther, const FHitResult& HitResult){
			auto* sceneComponent = dynamic_cast<JCollisionComponent*>(InOther);
			assert(sceneComponent);

			auto* Other = sceneComponent->GetParentSceneComponent();
			assert(Other);

			const ETraceType type = sceneComponent->GetTraceType();

			switch (type)
			{
			case ETraceType::BlockingVolume:
				{
					FVector RelativePosition = GetWorldLocation() - Other->GetWorldLocation();

					FVector correction = HitResult.HitNormal * HitResult.Distance * mDeltaTime;

					AddLocalLocation((RelativePosition.Dot(HitResult.HitNormal) < 0) ? -correction : correction);
				}
				break;
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
	mCollisionBox->Destroy();
}
