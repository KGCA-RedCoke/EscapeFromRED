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
	void Initialize() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

protected:
	void CheckGround();

protected:
	JPawnMovementComponent* mMovementComponent;
	JSphereComponent*       mCollisionSphere;
	JLineComponent*         mLineComponent;

	float mDeltaTime;
	float mMaxHeight = -9999999.f;
	float mYVelocity = 0.f;
};

REGISTER_CLASS_TYPE(APawn);
