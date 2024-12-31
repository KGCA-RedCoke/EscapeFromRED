#pragma once
#include "Core/Entity/Animation/JAnimator.h"

class AKillerClown;

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
    AKillerClown* mBoss = nullptr; // 소유 액터
};
