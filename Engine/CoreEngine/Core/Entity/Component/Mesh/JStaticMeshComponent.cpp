#include "JStaticMeshComponent.h"

#include "Core/Graphics/Mesh/JMeshObject.h"

JStaticMeshComponent::JStaticMeshComponent()
{}

JStaticMeshComponent::JStaticMeshComponent(JTextView InName)
	: JSceneComponent(InName)
{}

JStaticMeshComponent::~JStaticMeshComponent() {}


void JStaticMeshComponent::Tick(float DeltaTime)
{
	// TransformComponent에서 위치 업데이트
	JSceneComponent::Tick(DeltaTime);

	// MeshObject의 버퍼 업데이트
	if (mMeshObject)
	{
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
