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
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void         Initialize() override;
	void         Tick(float DeltaTime) override;
	void         Destroy() override;
	virtual void ReSpawn(const FVector& Location);

protected:
	void CheckGround();

public:
	friend class BtBase;

protected:
	JPawnMovementComponent* mMovementComponent;
	JSphereComponent*       mCollisionSphere;
	JSphereComponent*       mInteractionSphere;
	JLineComponent*         mLineComponent;

	void  SetYVelocity(float velocity);
	float mDeltaTime;
	float mMaxHeight  = -9999999.f;
	float mLastHeight = 0.f;
	float mYVelocity  = 0.f;
	bool  bIsLanding  = false;
};

REGISTER_CLASS_TYPE(APawn);
