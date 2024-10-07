#pragma once
#include "Core/Entity/JObject.h"

class JActor;

class JActorComponent : public JObject
{
public:
	JActorComponent();
	JActorComponent(JTextView InName);
	~JActorComponent() override;

	EObjectType GetObjectType() const override { return EObjectType::ActorComponent; }
	uint32_t    GetType() const override { return StringHash("ActorComponent"); }

	void Initialize() override;
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

public:
	FORCEINLINE bool IsActivated() const { return bIsActivated; }
	FORCEINLINE void SetActivated(bool bInActivated) { bIsActivated = bInActivated; }

	FORCEINLINE void        SetOwnerActor(const Ptr<JActor>& InActor) { mOwnerActor = InActor; }
	FORCEINLINE Ptr<JActor> GetOwnerActor() const { return mOwnerActor.lock(); }

	FORCEINLINE JArray<uint32_t>&       GetComponentTags() { return mComponentTags; }
	FORCEINLINE const JArray<uint32_t>& GetComponentTags() const { return mComponentTags; }

protected:
	WPtr<JActor> mOwnerActor;

	JArray<uint32_t> mComponentTags;
	bool             bIsActivated = true;

};
