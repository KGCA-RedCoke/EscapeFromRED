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

protected:
	class JSkeletalMeshComponent* mSkeletalMeshComponent;
	class JCameraComponent*       mFPSCamera;

protected:
	UPtr<XKeyboardMouse> mInput;
};

class ASampleCharacter : public ACharacter
{
public:
	ASampleCharacter();
	ASampleCharacter(JTextView InName, JTextView InMeshPath);
	~ASampleCharacter() override = default;

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

private:
	void SetupInputComponent() override;
	void OnMovementInputPressed(float DeltaTime, const FVector2& InDirection);
};
