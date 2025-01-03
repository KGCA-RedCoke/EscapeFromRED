#pragma once
#include "Core/Entity/Animation/JAnimator.h"

class AEnemy;

class JGirlAnimator : public JAnimator
{
public:
    JGirlAnimator();
    JGirlAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp = nullptr);
    ~JGirlAnimator() override = default;

public:
    void Initialize() override;
    void BeginPlay() override;

private:
    AEnemy* mEnemy = nullptr; // 소유 액터
};
