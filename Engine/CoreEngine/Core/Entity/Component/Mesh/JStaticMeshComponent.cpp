#include "JStaticMeshComponent.h"

#include "Core/Graphics/Mesh/JMeshObject.h"
extern FVector4 g_DirectionalLightPos;
extern FVector4 g_DirectionalLightColor;
JStaticMeshComponent::JStaticMeshComponent() {}

JStaticMeshComponent::JStaticMeshComponent(JTextView InName)
	: JSceneComponent(InName) {}

JStaticMeshComponent::~JStaticMeshComponent() {}


void JStaticMeshComponent::Tick(float DeltaTime)
{
	// TransformComponent에서 위치 업데이트
	JSceneComponent::Tick(DeltaTime);

	// MeshObject의 버퍼 업데이트
	if (mMeshObject)
	{
		mMeshObject->UpdateBuffer(mWorldMat, nullptr, g_DirectionalLightPos, g_DirectionalLightColor);
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
