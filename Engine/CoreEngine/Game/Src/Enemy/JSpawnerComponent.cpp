#include "JSpawnerComponent.h"

#include <DirectXColors.h>

#include "AEnemy.h"
#include "Core/Interface/JWorld.h"


JSpawnerComponent::JSpawnerComponent()
{
	mObjectType = NAME_COMPONENT_SPAWNER;
}

JSpawnerComponent::JSpawnerComponent(JTextView InName, AActor* InOwnerActor)
	: JActorComponent(InName, InOwnerActor)
{
	mObjectType = NAME_COMPONENT_SPAWNER;
}

JSpawnerComponent::~JSpawnerComponent() {}

uint32_t JSpawnerComponent::GetType() const
{
	return HASH_COMPONENT_TYPE_SPAWNER;
}

bool JSpawnerComponent::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JActorComponent::Serialize_Implement(FileStream))
	{
		return false;
	}

	// StartSpawn
	Utils::Serialization::Serialize_Primitive(&bStartSpawn, sizeof(bStartSpawn), FileStream);

	// Interval
	Utils::Serialization::Serialize_Primitive(&mInterval, sizeof(mInterval), FileStream);

	return true;
}

bool JSpawnerComponent::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!JActorComponent::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	// StartSpawn
	Utils::Serialization::DeSerialize_Primitive(&bStartSpawn, sizeof(bStartSpawn), InFileStream);

	// Interval
	Utils::Serialization::DeSerialize_Primitive(&mInterval, sizeof(mInterval), InFileStream);

	return true;
}

void JSpawnerComponent::ShowEditor()
{
	JActorComponent::ShowEditor();

	ImGui::Checkbox(u8("스폰 시작"), &bStartSpawn);
	ImGui::InputFloat(u8("스폰 간격"), &mInterval);
}

void JSpawnerComponent::Initialize()
{
	JActorComponent::Initialize();
}

void JSpawnerComponent::Tick(float DeltaTime)
{
	JActorComponent::Tick(DeltaTime);

	if (!bStartSpawn)
		return;

	mTime += DeltaTime;

	// 경과 시간이 일정 시간을 넘어가면 다시 스폰
	if (mTime >= mInterval)
	{
		mTime = 0.f;

		Spawn();
	}
}

void JSpawnerComponent::Spawn()
{
	AActor* newActor = GetWorld.LevelManager->GetActiveLevel()->CreateActor<AEnemy>("enemy");
	Utils::Serialization::DeSerialize("Game/Enemy/Enemy_KH.jasset", newActor);
	newActor->SetWorldLocation(GetOwnerActor()->GetWorldLocation());
	newActor->Initialize();
}
