#pragma once
#include "Core/Entity/Animation/JAnimator.h"

class JAudioComponent;
class AKillerClown;
class BT_BOSS;
class JKillerClownAnimator : public JAnimator
{
public:
    JKillerClownAnimator();
    JKillerClownAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp = nullptr);
    ~JKillerClownAnimator() override = default;

public:
    void Initialize() override;
    void BeginPlay() override;
	void Tick(float DeltaTime) override;

private:
    AKillerClown* mBoss = nullptr; // 소유 액터
    BT_BOSS* bt = nullptr;
    JAudioComponent* mLaughSound;
};
