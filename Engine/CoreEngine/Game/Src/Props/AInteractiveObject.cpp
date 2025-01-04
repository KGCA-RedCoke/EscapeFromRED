#include "AInteractiveObject.h"

AInteractiveObject::AInteractiveObject()
	: AActor(),
	  mInteractionComponent(nullptr)
{
	mObjectType = NAME_OBJECT_INTERACTIVE_OBJECT;
}

AInteractiveObject::AInteractiveObject(JTextView InName)
	: AActor(InName),
	  mInteractionComponent(nullptr)
{
	mObjectType = NAME_OBJECT_INTERACTIVE_OBJECT;
}

uint32_t AInteractiveObject::GetType() const
{
	return HASH_ASSET_TYPE_INTERACTIVE_OBJECT;
}

bool AInteractiveObject::Serialize_Implement(std::ofstream& FileStream)
{
	return AActor::Serialize_Implement(FileStream);
}

bool AInteractiveObject::DeSerialize_Implement(std::ifstream& InFileStream)
{
	bool bSuccess = AActor::DeSerialize_Implement(InFileStream);

	mInteractionComponent = static_cast<JSphereComponent*>(GetChildSceneComponentByType(NAME_COMPONENT_SPHERE));

	return bSuccess;
}

void AInteractiveObject::Initialize()
{
	if (!mInteractionComponent)
	{
		mInteractionComponent = CreateDefaultSubObject<JSphereComponent>("Interaction Collision", this);
		mInteractionComponent->SetupAttachment(this);
		mInteractionComponent->SetCollisionType(ECollisionType::Sphere);
		mInteractionComponent->SetTraceType(ETraceType::Interactive);
	}

	mInteractionComponent->OnComponentBeginOverlap.Bind([this](ICollision* OtherActor, const FHitResult& HitResult){
		auto* compType = dynamic_cast<JCollisionComponent*>(OtherActor);
		OnBeginOverlap.Execute(compType->GetOwnerActor());
	});

	EnrollInteractionComponent();

	AActor::Initialize();
}

void AInteractiveObject::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);

	switch (mInteractiveType) {
	case EInteractiveType::Cookie:
		
		break;
	case EInteractiveType::Chocolate:
		break;
	case EInteractiveType::Chicken:
		break;
	case EInteractiveType::Coke:
		break;
	case EInteractiveType::BossEnter_Clown:
		break;
	case EInteractiveType::BossEnter_Doll:
		break;
	case EInteractiveType::Max:
		break;
	}
}

void AInteractiveObject::Destroy()
{
	AActor::Destroy();
}

void AInteractiveObject::ShowEditor()
{
	AActor::ShowEditor();

	ImGui::SeparatorText(u8("충돌 설정"));

	ImGui::SeparatorText(u8("상호작용 설정"));
	if (ImGui::BeginCombo("Type", InteractiveTypeToString(mInteractiveType)))
	{
		for (int32_t i = 0; i < static_cast<int32_t>(EInteractiveType::Max); ++i)
		{
			bool isSelected = mInteractiveType == static_cast<EInteractiveType>(i);
			if (ImGui::Selectable(InteractiveTypeToString(static_cast<EInteractiveType>(i)), isSelected))
			{
				mInteractiveType = static_cast<EInteractiveType>(i);
			}
		}

		ImGui::EndCombo();
	}
}

void AInteractiveObject::EnrollInteractionComponent()
{
	switch (mInteractiveType)
	{
	case EInteractiveType::Cookie:
		OnBeginOverlap.Bind([](AActor* OtherActor){
			LOG_CORE_INFO("쿠키를 주시오");
		});
		break;
	case EInteractiveType::Chocolate:
		OnBeginOverlap.Bind([](AActor* OtherActor){
			LOG_CORE_INFO("초콜릿을 주시오");
		});
		break;
	case EInteractiveType::Chicken:
		OnBeginOverlap.Bind([](AActor* OtherActor){
			LOG_CORE_INFO("치킨을 주시오");
		});
		break;
	case EInteractiveType::Coke:
		OnBeginOverlap.Bind([](AActor* OtherActor){
			LOG_CORE_INFO("콜라를 주시오");
		});
		break;
	case EInteractiveType::BossEnter_Clown:
		OnBeginOverlap.Bind([](AActor* OtherActor){
			LOG_CORE_INFO("보스가 클라운이다");
		});
		break;
	case EInteractiveType::BossEnter_Doll:
		OnBeginOverlap.Bind([](AActor* OtherActor){
			LOG_CORE_INFO("보스가 인형이다");
		});
		break;
	case EInteractiveType::Max:
		break;
	}
}
