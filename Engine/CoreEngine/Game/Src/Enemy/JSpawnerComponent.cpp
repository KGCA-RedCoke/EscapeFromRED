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

	ImGui::InputInt(u8("최대 스폰 개수"), &mMaxSpawn);

	ImGui::InputFloat(u8("스폰 간격"), &mInterval);

	ImGui::TextColored(ImColor(1, 0, 0, 1),u8("몬스터 타입"));

	if (ImGui::BeginCombo("##Enemy Type", GetEnemyTypeString(static_cast<EEnemyType>(mEnemyType))))
	{
		for (uint8_t i = 0; i < EnumAsByte(EEnemyType::MAX); ++i)
		{
			if (ImGui::Selectable(GetEnemyTypeString(static_cast<EEnemyType>(i))))
			{
				mEnemyType = i;
				break;
			}
		}
		ImGui::EndCombo();
	}
}

void JSpawnerComponent::Initialize()
{
	JActorComponent::Initialize();
}

void JSpawnerComponent::BeginPlay()
{
	JActorComponent::BeginPlay();
	bStartSpawn = true;
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
	if (mMaxSpawn <= mSpawnCount)
	{
		bStartSpawn = false;
		return;
	}

	mSpawnCount++;

	JLevel*    level        = GetWorld.LevelManager->GetActiveLevel();
	EEnemyType enemyType    = static_cast<EEnemyType>(mEnemyType);
	JText      enemyTypeStr = GetEnemyTypeString(enemyType);
	JText      enemyName    = std::format("{}_{}", enemyTypeStr, mSpawnCount);
	auto       spawnedActor = level->mEnemyPool.Spawn(enemyName);
	switch (enemyType)
	{
	case EEnemyType::Kihyun:
		Utils::Serialization::DeSerialize("Game/Enemy/Enemy_KH.jasset", spawnedActor.get());
		break;
	}
	spawnedActor->SetWorldLocation(GetOwnerActor()->GetWorldLocation());
	spawnedActor->Initialize();
	
	level->mReservedActors.push_back(std::move(spawnedActor));
}
