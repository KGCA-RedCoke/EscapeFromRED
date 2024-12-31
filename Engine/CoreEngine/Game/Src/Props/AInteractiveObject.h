#pragma once
#include "Core/Entity/Actor/AActor.h"

enum class EInteractiveType
{
	Cookie,
	Chocolate,
	Chicken,
	Coke,
	BossEnter_Clown,
	BossEnter_Doll,
	Max
};

constexpr const char* InteractiveTypeToString(const EInteractiveType InType)
{
	switch (InType)
	{
	case EInteractiveType::Cookie:
		return "Cookie";
	case EInteractiveType::Chocolate:
		return "Chocolate";
	case EInteractiveType::Chicken:
		return "Chicken";
	case EInteractiveType::Coke:
		return "Coke";
	case EInteractiveType::BossEnter_Clown:
		return "BossEnter_Clown";
	case EInteractiveType::BossEnter_Doll:
		return "BossEnter_Doll";
	}

	return "Unknown";
}


DECLARE_DYNAMIC_DELEGATE(FOnBeginOverlap, AActor* OtherActor);

class AInteractiveObject : public AActor
{
public:
	FOnBeginOverlap OnBeginOverlap;

public:
	AInteractiveObject();
	AInteractiveObject(JTextView InName);
	~AInteractiveObject() override = default;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

public:
	void ShowEditor() override;

private:
	void EnrollInteractionComponent();

private:
	JSphereComponent* mInteractionComponent;
	EInteractiveType  mInteractiveType;
};

REGISTER_CLASS_TYPE(AInteractiveObject)
