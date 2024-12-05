#include "AActor.h"

#include "Core/Entity/Level/MLevelManager.h"
#include "Core/Interface/JWorld.h"
#include "GUI/MGUIManager.h"


AActor::AActor()
{
	mObjectType = NAME_OBJECT_ACTOR;
}

AActor::AActor(JTextView InName)
	: JSceneComponent(InName)
{
	mObjectType = NAME_OBJECT_ACTOR;
}

AActor::AActor(const AActor& Copy)
	: JSceneComponent(Copy)
{}

void AActor::Initialize()
{
	JSceneComponent::Initialize();
}

void AActor::BeginPlay()
{
	JSceneComponent::BeginPlay();
}

void AActor::Tick(float DeltaTime)
{
	JSceneComponent::Tick(DeltaTime);
}

void AActor::Destroy()
{
	JSceneComponent::Destroy();
}

void AActor::Draw()
{
	for (auto& sceneComponent : mChildSceneComponents)
	{

		if (sceneComponent)
		{
			sceneComponent->Draw();
		}
	}
}

uint32_t AActor::GetType() const
{
	// return JSceneComponent::GetType();
	return HASH_ASSET_TYPE_Actor;
}

bool AActor::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JSceneComponent::Serialize_Implement(FileStream))
	{
		return false;
	}


	return true;
}

bool AActor::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!JSceneComponent::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	return true;
}

JSceneComponent* AActor::GetChildSceneComponentByName(JTextView InName) const
{
	for (auto& sceneComponent : mChildSceneComponents)
	{
		if (sceneComponent->GetName() == InName)
		{
			return sceneComponent.get();
		}
	}

	return nullptr;
}

JSceneComponent* AActor::GetChildSceneComponentByType(JTextView InType) const
{
	const uint32_t type = StringHash(InType.data());

	for (auto& sceneComponent : mChildSceneComponents)
	{
		if (sceneComponent->GetType() == type)
		{
			return sceneComponent.get();
		}
	}
	return nullptr;
}

JActorComponent* AActor::GetChildComponentByType(JTextView InType) const
{
	const uint32_t type = StringHash(InType.data());

	for (auto& component : mActorComponents)
	{
		if (component->GetType() == type)
		{
			return component.get();
		}
	}

	return nullptr;
}
