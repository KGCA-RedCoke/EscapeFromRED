#pragma once
#include "Core/Entity/Actor/AActor.h"

class JSpawnerComponent : public JActorComponent
{
public:
	JSpawnerComponent();
	JSpawnerComponent(JTextView InName, AActor* InOwnerActor = nullptr);
	~JSpawnerComponent() override;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;
	void     ShowEditor() override;

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;

public:
	virtual void Spawn();
	void         StartSpawn() { bStartSpawn = true; }
	void         StopSpawn() { bStartSpawn = false; }
	void         SetInterval(float InInterval) { mInterval = InInterval; }

protected:
	bool bStartSpawn = false;

	float mInterval = 0.f;
	float mTime     = 0.f;
};

REGISTER_CLASS_TYPE(JSpawnerComponent)

class AEnermySpawner : public JSpawnerComponent
{};
