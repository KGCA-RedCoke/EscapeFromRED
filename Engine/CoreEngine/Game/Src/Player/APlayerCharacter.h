#pragma once
#include "Core/Entity/Actor/Character/ACharacter.h"


class JAnimationClip;

class APlayerCharacter : public ACharacter/*, public TSingleton<APlayerCharacter>*/
{
public:
	APlayerCharacter();
	APlayerCharacter(JTextView InName, JTextView InMeshPath);

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void ShowEditor() override;

private:
	void SetupInputComponent() override;
	void OnMovementInputPressed(float DeltaTime, const FVector& InDirection);

private:
	UPtr<JAnimationClip> mWalkAnimation;

};

REGISTER_CLASS_TYPE(APlayerCharacter);
