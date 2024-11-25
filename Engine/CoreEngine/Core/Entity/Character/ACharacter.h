#pragma once
#include "Core/Entity/Actor/JActor.h"

class XKeyboardMouse;

class ACharacter : public JActor
{
public:
	// Constructor
	ACharacter();
	// Constructor
	ACharacter(JTextView InName);
	// Destructor
	~ACharacter() override = default;

protected:
	virtual void SetupInputComponent() {};

protected:
	class JSkeletalMeshComponent* mSkeletalMeshComponent;
	class JMeshObject*            mMeshObject;

protected:
	UPtr<XKeyboardMouse> mInput;
};

class SampleCharacter : public ACharacter
{

	void SetupInputComponent() override;
	
};