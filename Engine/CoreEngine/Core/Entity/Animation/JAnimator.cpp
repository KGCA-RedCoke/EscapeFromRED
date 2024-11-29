#include "JAnimator.h"

uint32_t JAnimator::GetType() const
{
	return JActorComponent::GetType();
}

bool JAnimator::Serialize_Implement(std::ofstream& FileStream)
{
	return JActorComponent::Serialize_Implement(FileStream);
}

bool JAnimator::DeSerialize_Implement(std::ifstream& InFileStream)
{
	return JActorComponent::DeSerialize_Implement(InFileStream);
}

void JAnimator::Initialize()
{
	JActorComponent::Initialize();
}

void JAnimator::BeginPlay()
{
	JActorComponent::BeginPlay();
}

void JAnimator::Tick(float DeltaTime)
{
	JActorComponent::Tick(DeltaTime);
}

void JAnimator::Destroy()
{
	JActorComponent::Destroy();
}
