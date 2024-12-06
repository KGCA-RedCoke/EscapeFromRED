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

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void ShowEditor() override;

protected:
	virtual void SetupInputComponent() {};

public:
	FVector2 GetMouseDelta() const;

protected:
	class JSkeletalMeshComponent* mSkeletalMeshComponent;
	class JCameraComponent*       mFPSCamera;
	UPtr<XKeyboardMouse>          mInput;

};

REGISTER_CLASS_TYPE(ACharacter);
