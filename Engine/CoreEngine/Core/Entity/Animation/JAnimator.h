#pragma once
#include "Core/Entity/Component/JActorComponent.h"

class JAnimator : public JActorComponent
{
public:
	JAnimator()           = default;
	~JAnimator() override = default;

public:
	uint32_t GetType() const override;

public:
	bool Serialize_Implement(std::ofstream& FileStream) override;
	bool DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Initialize() override;
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void Destroy() override;

protected:
	// State Machine
	JHash<JText, Ptr<class JAnimationClip>> mStateMachine;

};
