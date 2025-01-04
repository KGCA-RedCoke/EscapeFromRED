#pragma once
#include "Core/Entity/Actor/Character/ACharacter.h"
#include "Game/Src/Enemy/AEnemy.h"

class JStaticMeshComponent;
class JAnimator;

enum class EBossState : uint8_t
{
    Idle,
    Walk,
    Run,
    Attack1,
    Attack2,
    Attack3,
    JumpAttack,
    Hit,
    Death,
    StandUp,
    MAX
};


class AKillerClown;
DECLARE_DYNAMIC_DELEGATE(FOnBossHit, const FHitResult& HitResult);

class AKillerClown : public AEnemy
{
public:
    FOnBossHit OnBossHit;

public:
    // Constructor
    AKillerClown();
    // Constructor
    AKillerClown(JTextView InName);
    // Destructor
    ~AKillerClown() override = default;

public:
    void Initialize() override;
    void Tick(float DeltaTime) override;
    void Destroy() override;

public:
    uint32_t GetType() const override;
    bool Serialize_Implement(std::ofstream& FileStream) override;
    bool DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
    void ShowEditor() override;

public:
    virtual void OnHit(ICollision* InActor, const FHitResult& HitResult);
    void SetBossState(EBossState InNewState) { mBossState = InNewState; }
    EBossState GetBossState() { return mBossState; }

protected:
    EBossState mBossState;
    JStaticMeshComponent* mHammerMesh;
    JSphereComponent* mHammerRegion;
    // JSphereComponent* mWeaponCollider;
    friend class JKillerClownAnimator;

    float HP = 100;
};

REGISTER_CLASS_TYPE(AKillerClown)
