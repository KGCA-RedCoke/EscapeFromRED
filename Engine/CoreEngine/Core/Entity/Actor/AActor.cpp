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

	uint32_t numComponents = mActorComponents.size();
	Utils::Serialization::Serialize_Primitive(&numComponents, sizeof(uint32_t), FileStream);
	for (auto& component : mActorComponents)
	{
		JText objectType = component->GetObjectType();
		Utils::Serialization::Serialize_Text(objectType, FileStream);
		component->Serialize_Implement(FileStream);
	}

	return true;
}

bool AActor::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!JSceneComponent::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	const int32_t currentFilePos = InFileStream.tellg();

	JAssetHeader header;
	Utils::Serialization::DeserializeHeader(InFileStream, header);
	InFileStream.seekg(currentFilePos);


	if (header.Version == 2)
	{
		uint32_t numComponents = 0;
		Utils::Serialization::DeSerialize_Primitive(&numComponents, sizeof(uint32_t), InFileStream);
		mActorComponents.reserve(numComponents);
		for (uint32_t i = 0; i < numComponents; ++i)
		{
			JText objectType;
			Utils::Serialization::DeSerialize_Text(objectType, InFileStream);
			auto newComp = UPtrCast<JActorComponent>(MClassFactory::Get().Create(objectType));
			newComp->DeSerialize_Implement(InFileStream);
			newComp->SetOwnerActor(this);
			mActorComponents.push_back(std::move(newComp));
		}
	}

	return true;
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
