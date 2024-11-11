#include "JStaticMeshActor.h"

#include "Core/Entity/Component/Mesh/JStaticMeshComponent.h"
#include "Core/Graphics/Mesh/JMeshObject.h"
#include "Core/Interface/MManagerInterface.h"

JStaticMeshActor::JStaticMeshActor(JTextView InName, const Ptr<JMeshObject>& InMeshObject)
	: JActor(InName),
	  mMeshObject(InMeshObject)
{}

JStaticMeshActor::JStaticMeshActor(JTextView InName, JTextView SavedMeshPath)
	: JActor(InName)
{
	if (const Ptr<JMeshObject> meshObject = IManager.MeshManager->CreateOrClone(SavedMeshPath.data()))
	{
		mMeshObject = meshObject;
	}
}

void JStaticMeshActor::Initialize()
{
	JActor::Initialize();

	CreateMeshComponent(mMeshObject);
}

void JStaticMeshActor::CreateMeshComponent(const Ptr<JMeshObject>& InMeshObject)
{
	// 메시 할당
	Ptr<JStaticMeshComponent> meshComponent = CreateDefaultSubObject<JStaticMeshComponent>(InMeshObject->GetName());
	meshComponent->SetMeshObject(InMeshObject);

	// 메시 컴포넌트 부착 (루트에 부착)
	meshComponent->SetupAttachment(mRootComponent);
}
