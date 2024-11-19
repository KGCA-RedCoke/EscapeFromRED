#include "JStaticMeshComponent.h"

#include "Core/Graphics/Mesh/JMeshObject.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Interface/JWorld.h"

JStaticMeshComponent::JStaticMeshComponent()
{
	mObjectType = NAME_OBJECT_STATIC_MESH_COMPONENT;
}

JStaticMeshComponent::JStaticMeshComponent(JTextView        InName,
										   JActor*          InOwnerActor,
										   JSceneComponent* InParentSceneComponent)
	: JSceneComponent(InName, InOwnerActor, InParentSceneComponent)
{
	mObjectType = NAME_OBJECT_STATIC_MESH_COMPONENT;
}

JStaticMeshComponent::~JStaticMeshComponent() {}

bool JStaticMeshComponent::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JSceneComponent::Serialize_Implement(FileStream))
	{
		return false;
	}

	bool bHasMeshObject = mMeshObject != nullptr;
	Utils::Serialization::Serialize_Primitive(&bHasMeshObject, sizeof(bool), FileStream);
	if (bHasMeshObject)
	{
		mMeshObject->Serialize_Implement(FileStream);
	}

	return true;
}

bool JStaticMeshComponent::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!JSceneComponent::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	bool bHasMeshObject;
	Utils::Serialization::DeSerialize_Primitive(&bHasMeshObject, sizeof(bool), InFileStream);

	if (bHasMeshObject)
	{
		mMeshObject = MakeUPtr<JMeshObject>();
		mMeshObject->DeSerialize_Implement(InFileStream);
	}


	return true;
}


void JStaticMeshComponent::Tick(float DeltaTime)
{
	// TransformComponent에서 위치 업데이트
	JSceneComponent::Tick(DeltaTime);

	// MeshObject의 버퍼 업데이트
	if (mMeshObject)
	{
		mMeshObject->Tick(DeltaTime);
		mMeshObject->UpdateBuffer(mWorldMat);
	}
}

void JStaticMeshComponent::Draw()
{
	// MeshObject의 Draw 호출
	if (mMeshObject)
	{
		mMeshObject->Draw();
	}

	// Child SceneComponent Draw 호출
	JSceneComponent::Draw();
}

void JStaticMeshComponent::DrawID(uint32_t ID)
{
	if (mMeshObject)
	{
		mMeshObject->DrawID(ID);
	}

	JSceneComponent::DrawID(ID);
}

void JStaticMeshComponent::SetMaterialInstance(JMaterialInstance* InMaterialInstance, uint32_t InSlot)
{
	if (!mMeshObject)
	{
		LOG_CORE_ERROR("MeshObject가 없음.");
		return;
	}

	mMeshObject->SetMaterialInstance(InMaterialInstance, InSlot);
}

void JStaticMeshComponent::SetMeshObject(JTextView InMeshObject)
{
	mMeshObject = UPtrCast<JMeshObject>(GetWorld.MeshManager->CreateOrLoad(InMeshObject.data())->Clone());
}
