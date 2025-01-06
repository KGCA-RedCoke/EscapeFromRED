#pragma once

#include "Core/Entity/Animation/JAnimator.h"
class AEnemy;

class JMadreAnimator : public JAnimator
{
public:
    JMadreAnimator();
    JMadreAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp = nullptr);
    ~JMadreAnimator() override = default;

public:
    void Initialize() override;
    void BeginPlay() override;

private:
    AEnemy* mEnemy = nullptr; // 소유 액터
};
