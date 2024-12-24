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

protected:
	JPawnMovementComponent* mMovementComponent;
};

REGISTER_CLASS_TYPE(APawn);
