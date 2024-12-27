#pragma once
#include "Core/Entity/Actor/Character/ACharacter.h"

class JAnimator;
class JStaticMeshComponent;

class APlayerCharacter : public ACharacter
{
public:
	APlayerCharacter();
	APlayerCharacter(JTextView InName, JTextView InMeshPath);

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

private:
	void SetupInputComponent() override;
	void UpdateRotation();
	void OnMovementInputPressed(float DeltaTime, const FVector& InDirection);
	void CheckGround();

private:
	class JCameraComponent* mFPSCamera;
	JStaticMeshComponent*   mLightMesh;

	FVector2 mMouseDelta;
	FVector2 mRotVelocity;

	JArray<class JAnimationClip*> mAnimList;
	uint32_t                      mCurrentAnimIndex = 0;
	bool                          bMove             = false;
	UPtr<JAnimator>               mPlayerAnimator;

};

REGISTER_CLASS_TYPE(APlayerCharacter);
