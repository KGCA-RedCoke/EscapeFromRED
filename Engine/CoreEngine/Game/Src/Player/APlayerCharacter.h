#pragma once
#include "Core/Entity/Actor/Character/ACharacter.h"

// #define GET_PLAYER APlayerCharacter::Get()

class APlayerCharacter : public ACharacter/*, public TSingleton<APlayerCharacter>*/
{
public:
	APlayerCharacter(JTextView InName, JTextView InMeshPath);

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

private:
	void SetupInputComponent() override;
	void OnMovementInputPressed(float DeltaTime, const FVector& InDirection);


// #pragma region Singleton Boilerplate
//
// private:
// 	friend class TSingleton<APlayerCharacter>;
//
// 	APlayerCharacter()           = default;
// 	~APlayerCharacter() override = default;
//
// public:
// 	APlayerCharacter(const APlayerCharacter&)            = delete;
// 	APlayerCharacter& operator=(const APlayerCharacter&) = delete;
//
// #pragma endregion

};
