#pragma once
#include "AActor.h"

class JPawnMovementComponent;

class APawn : public AActor
{
public:
    APawn();
    APawn(JTextView Name);
    ~APawn() override;

public:
    uint32_t GetType() const override;
    bool Serialize_Implement(std::ofstream& FileStream) override;
    bool DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
    void Initialize() override;
    void Tick(float DeltaTime) override;
    void Destroy() override;

    void HandlePawnToPawn(const FHitResult& HitResult);
protected:
    JPawnMovementComponent* mMovementComponent;
    JBoxComponent* mCollisionBox;
    JLineComponent* mLineComponent;

    float mDeltaTime;
    float mMaxHeight = 100.f;
};

REGISTER_CLASS_TYPE(APawn);
