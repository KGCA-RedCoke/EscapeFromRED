#pragma once

#include "Core/Entity/Animation/JAnimator.h"

class AEnemy;

class JPigAnimator : public JAnimator
{
public:
    JPigAnimator();
    JPigAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp = nullptr);
    ~JPigAnimator() override = default;

public:
    void Initialize() override;
    void BeginPlay() override;

private:
    AEnemy* mEnemy = nullptr; // 소유 액터

};
