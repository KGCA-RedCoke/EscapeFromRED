#include "JActorComponent.h"

JActorComponent::JActorComponent()
{}

JActorComponent::JActorComponent(JTextView InName)
	: JObject(InName) {}

JActorComponent::~JActorComponent()
{}

void JActorComponent::Initialize()
{
	JObject::Initialize();
}

void JActorComponent::BeginPlay()
{
	JObject::BeginPlay();
}

void JActorComponent::Tick(float DeltaTime)
{
	JObject::Tick(DeltaTime);
}

void JActorComponent::Destroy()
{
	JObject::Destroy();
}
