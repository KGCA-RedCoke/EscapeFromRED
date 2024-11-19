#pragma once
#include "Core/Entity/JObject.h"

class JActor;

class JActorComponent : public JObject
{
public:
	JActorComponent();
	JActorComponent(JTextView InName);
	~JActorComponent() override;

public:
	uint32_t GetType() const override { return StringHash("ActorComponent"); }

	bool Serialize_Implement(std::ofstream& FileStream) override;
	bool DeSerialize_Implement(std::ifstream& InFileStream) override;

	void Initialize() override;
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

public:
	FORCEINLINE bool IsActivated() const { return bIsActivated; }
	FORCEINLINE void SetActivated(bool bInActivated) { bIsActivated = bInActivated; }

	FORCEINLINE void    SetOwnerActor(JActor* InActor) { mOwnerActor = InActor; }
	FORCEINLINE JActor* GetOwnerActor() const { return mOwnerActor; }

	FORCEINLINE JArray<uint32_t>&       GetComponentTags() { return mComponentTags; }
	FORCEINLINE const JArray<uint32_t>& GetComponentTags() const { return mComponentTags; }

protected:
	JActor* mOwnerActor;

	JArray<uint32_t> mComponentTags;
	bool             bIsActivated = true;
};

REGISTER_CLASS_TYPE(JActorComponent)
