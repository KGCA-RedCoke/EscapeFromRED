#include "JStaticMeshActor.h"

#include "Core/Entity/Component/Mesh/JStaticMeshComponent.h"
#include "Core/Graphics/Mesh/JMeshObject.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Interface/JWorld.h"

JStaticMeshActor::JStaticMeshActor()
{
	mObjectType = NAME_OBJECT_STATIC_MESH_ACTOR;
}

JStaticMeshActor::JStaticMeshActor(JTextView InName, JTextView SavedMeshPath)
	: AActor(InName)
{
	mObjectType = NAME_OBJECT_STATIC_MESH_ACTOR;

	CreateMeshComponent(SavedMeshPath);

}

JStaticMeshActor::JStaticMeshActor(const JStaticMeshActor& Copy)
	: AActor(Copy)
{
	CreateMeshComponent(Copy.mStaticMeshComponent->GetName());
}

UPtr<IManagedInterface> JStaticMeshActor::Clone() const
{
	return MakeUPtr<JStaticMeshActor>(*this);
}


void JStaticMeshActor::Initialize()
{
	AActor::Initialize();

}

bool JStaticMeshActor::Serialize_Implement(std::ofstream& FileStream)
{
	if (!AActor::Serialize_Implement(FileStream))
	{
		return false;
	}


	return true;
}

bool JStaticMeshActor::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!AActor::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	mStaticMeshComponent = static_cast<JStaticMeshComponent*>(GetChildSceneComponentByType("StaticMeshComponent"));

	return true;
}

int32_t JStaticMeshActor::GetMaterialCount() const
{
	if (!mStaticMeshComponent)
	{
		LOG_CORE_ERROR("유효하지 않은 메시 오브젝트.");
		return 0;
	}

	return mStaticMeshComponent->GetMaterialCount();
}

void JStaticMeshActor::SetMaterialInstance(JMaterialInstance* InMaterialInstance, uint32_t InIndex)
{
	if (!mStaticMeshComponent)
	{
		LOG_CORE_ERROR("유효하지 않은 메시 오브젝트.");
		return;
	}

	mStaticMeshComponent->SetMaterialInstance(InMaterialInstance, InIndex);
}

void JStaticMeshActor::CreateMeshComponent(JTextView InMeshObject)
{
	// 메시 할당
	mStaticMeshComponent = CreateDefaultSubObject<JStaticMeshComponent>(InMeshObject, this, this);
	mStaticMeshComponent->SetMeshObject(InMeshObject);

	// 메시 컴포넌트 부착 (루트에 부착)
	// mStaticMeshComponent->SetupAttachment(mRootComponent);
}
