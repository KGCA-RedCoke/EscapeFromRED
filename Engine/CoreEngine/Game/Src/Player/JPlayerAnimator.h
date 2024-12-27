#pragma once
#include "Core/Entity/Animation/JAnimator.h"

enum class EPlayerState : uint8_t
{
	Idle = 0,
	Walk,
	Run,
	Crouch,
	Jump,
	Attack,
	Dead
};

class JPlayerAnimator : public JAnimator
{
public:
	JPlayerAnimator();
	JPlayerAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp = nullptr);
	JPlayerAnimator(JTextView InName, AActor* InOwnerActor = nullptr);
	~JPlayerAnimator() override = default;

public:
	void Initialize() override;
	void BeginPlay() override;

};
