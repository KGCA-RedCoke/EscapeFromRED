#pragma once
#include "Core/Entity/Actor/Character/ACharacter.h"

class JAnimator;
class JStaticMeshComponent;

class APlayerCharacter : public ACharacter
{
public:
	APlayerCharacter();
	APlayerCharacter(JTextView InName, JTextView InMeshPath);
	~APlayerCharacter() override = default;

public:
	void Initialize() override;
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

private:
	void SetupInputComponent() override;
	void UpdateRotation();
	void OnMovementInputPressed(float DeltaTime, const FVector& InDirection);
	void OnMeleeAttack();
	void DisableMeleeCollision();
	void OnMeleeAttackFinished();

private:
	class JCameraComponent* mFPSCamera;
	JStaticMeshComponent*   mWeaponMesh;
	JSphereComponent*          mWeaponCollision;

	FVector2 mMouseDelta;
	FVector2 mRotVelocity;

	bool            bShouldAttack = false;
	bool            bShouldRun    = false;
	uint32_t        mAttackCombo  = 0;
	UPtr<JAnimator> mPlayerAnimator;

	friend class JPlayerAnimator;
};

REGISTER_CLASS_TYPE(APlayerCharacter);
