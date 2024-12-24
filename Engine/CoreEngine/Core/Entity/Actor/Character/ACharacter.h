#pragma once
#include "Core/Entity/Actor/APawn.h"

class XKeyboardMouse;

class ACharacter : public APawn
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
	float    GetYaw() const { return mYaw; }
	float    GetPitch() const { return mPitch; }
	FVector2 GetMouseDelta() const;

protected:
	class JSkeletalMeshComponent* mSkeletalMeshComponent;
	UPtr<XKeyboardMouse>          mInput;

	float   mLastYaw = 0;
	float   mYaw     = 0;
	float   mPitch   = 0;
	FVector mForward;

};

REGISTER_CLASS_TYPE(ACharacter);
