#include "JActor.h"

#include "Core/Entity/Level/MLevelManager.h"
#include "Core/Interface/JWorld.h"
#include "GUI/MGUIManager.h"


JActor::JActor()
{
	mObjectType = NAME_OBJECT_ACTOR;
}

JActor::JActor(JTextView InName)
	: JSceneComponent(InName)
{
	mObjectType = NAME_OBJECT_ACTOR;
}

void JActor::Initialize()
{
	JSceneComponent::Initialize();
}

void JActor::BeginPlay()
{
	JSceneComponent::BeginPlay();
}

void JActor::Tick(float DeltaTime)
{
	JSceneComponent::Tick(DeltaTime);
}

void JActor::Destroy()
{
	JSceneComponent::Destroy();
}

void JActor::Draw()
{
	for (auto& sceneComponent : mChildSceneComponents)
	{
		if (sceneComponent)
		{
			sceneComponent->Draw();
		}
	}
}

uint32_t JActor::GetType() const
{
	return JSceneComponent::GetType();
}

bool JActor::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JSceneComponent::Serialize_Implement(FileStream))
	{
		return false;
	}


	return true;
}

bool JActor::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!JSceneComponent::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	return true;
}
