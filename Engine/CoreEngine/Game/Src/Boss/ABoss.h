#pragma once
#include "Core/Entity/Actor/Character/ACharacter.h"

class JAnimator;

enum class EBossType : uint8_t
{
    Kihyun,
    Girl,
    Clown,
    Custom,
    MAX // 이건 숫자 사용
};

enum class EBossState : uint8_t
{
    Idle,
    Walk,
    Run,
    Attack,
    Death,
    MAX
};

constexpr const char* GetEnemyTypeString(EBossType InType)
{
    switch (InType)
    {
    case EBossType::Kihyun:
        return "Kihyun";
    case EBossType::Girl:
        return "Girl";
    case EBossType::Clown:
        return "Clown";
    case EBossType::Custom:
        break;
    case EBossType::MAX:
        break;
    }
    return "Unknown";
}

class ABoss;
DECLARE_DYNAMIC_DELEGATE(FOnBossHit, const FHitResult& HitResult);

class ABoss : public APawn
{
public:
    FOnBossHit OnBossHit;

public:
    // Constructor
    ABoss();
    // Constructor
    ABoss(JTextView InName);
    // Destructor
    ~ABoss() override = default;

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
    void SetEnemyState(EBossState InNewState) { mBossState = InNewState; }
    EBossState GetBossState() { return mBossState; }

protected:
    EBossType mBossType;
    EBossState mBossState;
    UPtr<JAnimator> mAnimator;
    class JSkeletalMeshComponent* mSkeletalMeshComponent;
    class BT_BOSS* mBehaviorTree;

    friend class JKillerClownAnimator;
};

REGISTER_CLASS_TYPE(ABoss)
