#include "JActorComponent.h"

JActorComponent::JActorComponent()
	: mOwnerActor(nullptr)
{
	mObjectType = NAME_OBJECT_ACTOR_COMPONENT;
}

JActorComponent::JActorComponent(JTextView InName, AActor* InOwnerActor)
	: JObject(InName),
	  mOwnerActor(InOwnerActor)
{
	mObjectType = NAME_OBJECT_ACTOR_COMPONENT;
}

JActorComponent::JActorComponent(const JActorComponent& Copy)
	: JObject(Copy),
	  mComponentTags(Copy.mComponentTags),
	  bIsActivated(Copy.bIsActivated)
{
	mOwnerActor = nullptr;
}

JActorComponent::~JActorComponent() = default;

bool JActorComponent::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JObject::Serialize_Implement(FileStream))
	{
		return false;
	}

	// Tags
	int32_t tagCount = mComponentTags.size();
	Utils::Serialization::Serialize_Primitive(&tagCount, sizeof(int32_t), FileStream);
	for (int32_t i = 0; i < tagCount; ++i)
	{
		Utils::Serialization::Serialize_Primitive(&mComponentTags[i], sizeof(uint32_t), FileStream);
	}

	Utils::Serialization::Serialize_Primitive(&bIsActivated, sizeof(bool), FileStream);

	return true;
}

bool JActorComponent::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!JObject::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	int32_t tagCount;
	Utils::Serialization::DeSerialize_Primitive(&tagCount, sizeof(int32_t), InFileStream);
	for (int32_t i = 0; i < tagCount; ++i)
	{
		uint32_t tag;
		Utils::Serialization::DeSerialize_Primitive(&tag, sizeof(uint32_t), InFileStream);
		mComponentTags.push_back(tag);
	}

	Utils::Serialization::DeSerialize_Primitive(&bIsActivated, sizeof(bool), InFileStream);

	return true;
}

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

void JActorComponent::ShowEditor()
{}
