#pragma once
#include "Core/Entity/JObject.h"
#include "Core/Interface/IGUIEditable.h"

class AActor;

class JActorComponent : public JObject, public IGUIEditable
{
public:
	JActorComponent();
	JActorComponent(JTextView InName, AActor* InOwnerActor = nullptr);
	JActorComponent(const JActorComponent& Copy);
	~JActorComponent() override;

public:
	uint32_t GetType() const override { return StringHash("ActorComponent"); }

	bool Serialize_Implement(std::ofstream& FileStream) override;
	bool DeSerialize_Implement(std::ifstream& InFileStream) override;

	void Initialize() override;
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

	void ShowEditor() override;

public:
	FORCEINLINE bool IsActivated() const { return bIsActivated; }
	FORCEINLINE void SetActivated(bool bInActivated) { bIsActivated = bInActivated; }

	FORCEINLINE void    SetOwnerActor(AActor* InActor) { mOwnerActor = InActor; }
	FORCEINLINE AActor* GetOwnerActor() const { return mOwnerActor; }

	FORCEINLINE JArray<uint32_t>&       GetComponentTags() { return mComponentTags; }
	FORCEINLINE const JArray<uint32_t>& GetComponentTags() const { return mComponentTags; }

protected:
	AActor* mOwnerActor;

	JArray<uint32_t> mComponentTags;
	bool             bIsActivated = true;
};

REGISTER_CLASS_TYPE(JActorComponent)
