#pragma once
#include "../AActor.h"

class XKeyboardMouse;

class ACharacter : public AActor
{
public:
	// Constructor
	ACharacter();
	// Constructor
	ACharacter(JTextView InName);
	// Destructor
	~ACharacter() override = default;

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

protected:
	virtual void SetupInputComponent() {};

public:
	FVector2 GetMouseDelta() const;

protected:
	class JSkeletalMeshComponent* mSkeletalMeshComponent;
	class JCameraComponent*       mFPSCamera;
	UPtr<XKeyboardMouse>          mInput;
	FVector2                      mMouseDelta;
	FVector2                      mRotVelocity;

};
