#pragma once
#include "Core/Entity/Camera/JCameraComponent.h"

class ACharacter;

class JPlayerCamera : public JCameraComponent
{
public:
	JPlayerCamera();
	JPlayerCamera(JTextView InName, ACharacter* InOwner, JSceneComponent* InParent);
	~JPlayerCamera() = default;

public:
	void Tick(float DeltaTime) override;

public:
	void UpdateRotation(float DeltaTime) override;
	void UpdateVelocity(float DeltaTime) override;

private:
	ACharacter* mOwnerCharacter;

};
