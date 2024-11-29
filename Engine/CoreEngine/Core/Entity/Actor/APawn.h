#pragma once
#include "AActor.h"

class JPawnMovementComponent;

class APawn : public AActor
{
public:
	APawn(JTextView Name);
	~APawn() override;

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

protected:
	JPawnMovementComponent* mMovementComponent;
};
