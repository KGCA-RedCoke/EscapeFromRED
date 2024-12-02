#pragma once
#include "Core/Entity/Actor/Character/ACharacter.h"

class APlayerCharacter : public ACharacter
{
public:
	APlayerCharacter();
	APlayerCharacter(JTextView InName, JTextView InMeshPath);
	~APlayerCharacter() override = default;

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

private:
	void SetupInputComponent() override;
	void OnMovementInputPressed(float DeltaTime, const FVector& InDirection);

};
