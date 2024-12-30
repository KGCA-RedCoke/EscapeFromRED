#pragma once
#include "Core/Entity/Actor/AActor.h"

class AInteractiveObject : public AActor
{
public:
	AInteractiveObject();
	AInteractiveObject(JTextView InName);
	~AInteractiveObject() override = default;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

public:
	void ShowEditor() override;

private:
	JBoxComponent* mBoxComponent;
};
