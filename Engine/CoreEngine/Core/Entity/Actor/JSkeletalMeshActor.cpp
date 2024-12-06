#include "JSkeletalMeshActor.h"

#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"

JSkeletalMeshActor::JSkeletalMeshActor()
{
	mObjectType = NAME_OBJECT_SKELETAL_MESH_ACTOR;
}

JSkeletalMeshActor::JSkeletalMeshActor(JTextView InName, JTextView SavedMeshPath)
	: AActor(InName)
{
	mObjectType = NAME_OBJECT_SKELETAL_MESH_ACTOR;
	CreateMeshComponent(SavedMeshPath);
}

JSkeletalMeshActor::JSkeletalMeshActor(const JSkeletalMeshActor& Copy)
{
	CreateMeshComponent(Copy.mSkeletalMeshComponent->GetName());
}

UPtr<IManagedInterface> JSkeletalMeshActor::Clone() const
{
	return MakeUPtr<JSkeletalMeshActor>(*this);
}

void JSkeletalMeshActor::Initialize()
{
	AActor::Initialize();
}

bool JSkeletalMeshActor::Serialize_Implement(std::ofstream& FileStream)
{
	return AActor::Serialize_Implement(FileStream);
}

bool JSkeletalMeshActor::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!AActor::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	mSkeletalMeshComponent = static_cast<JSkeletalMeshComponent*>(GetChildSceneComponentByType("StaticMeshComponent"));
	mBoundingBox           = mSkeletalMeshComponent->GetBoundingVolume();
	UpdateTransform();

	return true;
}

int32_t JSkeletalMeshActor::GetMaterialCount() const
{
	if (!mSkeletalMeshComponent)
	{
		LOG_CORE_ERROR("유효하지 않은 메시 오브젝트.");
		return 0;
	}

	return 1;
	// return mSkeletalMeshComponent->GetMaterialCount();
}

void JSkeletalMeshActor::SetMaterialInstance(JMaterialInstance* InMaterialInstance, uint32_t InIndex) {}

void JSkeletalMeshActor::CreateMeshComponent(JTextView InMeshObject)
{
	// 메시 할당
	mSkeletalMeshComponent = CreateDefaultSubObject<JSkeletalMeshComponent>(InMeshObject, this, this);
	mSkeletalMeshComponent->SetSkeletalMesh(InMeshObject);
	mBoundingBox = mSkeletalMeshComponent->GetBoundingVolume();
}
