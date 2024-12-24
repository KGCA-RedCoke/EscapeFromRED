#pragma once
#include "Core/Entity/Animation/JAnimator.h"

class JPlayerAnimator : public JAnimator
{

public:
	JPlayerAnimator();
	JPlayerAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp = nullptr);
	JPlayerAnimator(JTextView InName, AActor* InOwnerActor = nullptr);
	~JPlayerAnimator() override = default;

public:
	void Initialize();

};
