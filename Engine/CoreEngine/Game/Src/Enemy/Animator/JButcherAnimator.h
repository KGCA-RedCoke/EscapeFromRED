#pragma once

#include "Core/Entity/Animation/JAnimator.h"

class AEnemy;
class BT_Butcher;

class JButcherAnimator : public JAnimator
{
public:
    JButcherAnimator();
    JButcherAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp = nullptr);
    ~JButcherAnimator() override = default;

public:
    void Initialize() override;
    void BeginPlay() override;

private:
    AEnemy* mEnemy = nullptr; // 소유 액터
    BT_Butcher* bt = nullptr;
};
