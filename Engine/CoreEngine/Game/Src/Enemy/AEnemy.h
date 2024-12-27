#pragma once
#include "Core/Entity/Actor/Character/ACharacter.h"

class JAnimator;

enum class EEnemyType : uint8_t
{
    Kihyun,
    Girl,
    Clown,
    Custom,
    MAX // 이건 숫자 사용
};

constexpr const char* GetEnemyTypeString(EEnemyType InType)
{
    switch (InType)
    {
    case EEnemyType::Kihyun:
        return "Kihyun";
    case EEnemyType::Girl:
        return "Girl";
    case EEnemyType::Clown:
        return "Clown";
    case EEnemyType::Custom:
        break;
    case EEnemyType::MAX:
        break;
    }
    return "Unknown";
}


class AEnemy : public APawn
{
public:
    // Constructor
    AEnemy();
    // Constructor
    AEnemy(JTextView InName);
    // Destructor
    ~AEnemy() override = default;

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

protected:
    EEnemyType mEnemyType;
    UPtr<JAnimator> mAnimator;
    class JSkeletalMeshComponent* mSkeletalMeshComponent;
    class BT_BOSS* mBehaviorTree;
};

REGISTER_CLASS_TYPE(AEnemy)