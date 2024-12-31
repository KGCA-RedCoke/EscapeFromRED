#pragma once
#include "Core/Entity/Animation/JAnimator.h"

class ABoss;

class JKillerClownAnimator : public JAnimator
{
public:
    JKillerClownAnimator();
    JKillerClownAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp = nullptr);
    ~JKillerClownAnimator() override = default;

public:
    void Initialize() override;
    void BeginPlay() override;

private:
    ABoss* mBoss = nullptr; // 소유 액터
};
