#pragma once
#include "Core/Entity/Animation/JAnimator.h"

class AEnemy;

class JKihyunAnimator : public JAnimator
{
public:
	JKihyunAnimator();
	JKihyunAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp = nullptr);
	~JKihyunAnimator() override = default;

public:
	void Initialize() override;
	void BeginPlay() override;

private:
	AEnemy* mEnemy = nullptr; // 소유 액터
};
