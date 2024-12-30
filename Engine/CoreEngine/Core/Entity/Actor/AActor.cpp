#include "AActor.h"

#include "Core/Entity/Level/MLevelManager.h"
#include "Core/Graphics/Vertex/XTKPrimitiveBatch.h"
#include "Core/Interface/JWorld.h"
#include "GUI/MGUIManager.h"


AActor::AActor()
	: mParentActor(nullptr)
{
	mObjectType = NAME_OBJECT_ACTOR;
}

AActor::AActor(JTextView InName)
	: JSceneComponent(InName),
	  mParentActor(nullptr)
{
	mObjectType = NAME_OBJECT_ACTOR;
}

AActor::AActor(const AActor& Copy)
	: JSceneComponent(Copy),
	  mParentActor(Copy.mParentActor)
{}

void AActor::Initialize()
{
	for (const auto& actorComponent : mActorComponents)
	{
		actorComponent->Initialize();
	}
	JSceneComponent::Initialize();
}

void AActor::BeginPlay()
{
	JSceneComponent::BeginPlay();
}

void AActor::Tick(float DeltaTime)
{
	JSceneComponent::Tick(DeltaTime);

	mBoundingBox.Box.LocalAxis[0] = XMVector3TransformNormal(FVector(1, 0, 0), XMLoadFloat4x4(&mWorldRotationMat));
	mBoundingBox.Box.LocalAxis[1] = XMVector3TransformNormal(FVector(0, 1, 0), XMLoadFloat4x4(&mWorldRotationMat));
	mBoundingBox.Box.LocalAxis[2] = XMVector3TransformNormal(FVector(0, 0, 1), XMLoadFloat4x4(&mWorldRotationMat));
	mBoundingBox.Box.Center       = XMVector3Transform(FVector::ZeroVector, mWorldLocationMat);

	for (const auto& actorComponent : mActorComponents)
	{
		actorComponent->Tick(DeltaTime);
	}
}

void AActor::Destroy()
{
	JSceneComponent::Destroy();

	for (const auto& actorComponent : mActorComponents)
	{
		actorComponent->Destroy();
	}
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

void AActor::ShowEditor()
{
	JSceneComponent::ShowEditor();

	for (auto& actorComponent : mActorComponents)
	{
		ImGui::SeparatorText(actorComponent->GetName().data());
		actorComponent->ShowEditor();
	}
}

uint32_t AActor::GetType() const
{
	// return JSceneComponent::GetCollisionType();
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
