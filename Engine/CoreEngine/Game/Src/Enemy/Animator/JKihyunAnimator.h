#pragma once
#include "Core/Entity/Animation/JAnimator.h"

class JKihyunAnimator : public JAnimator
{
public:
    JKihyunAnimator();
    JKihyunAnimator(JTextView InName, JSkeletalMeshComponent* InSkeletalComp = nullptr);
    JKihyunAnimator(JTextView InName, AActor* InOwnerActor = nullptr);
    ~JKihyunAnimator() override = default;

    
public:
    void Initialize() override;
    void BeginPlay() override;
};
