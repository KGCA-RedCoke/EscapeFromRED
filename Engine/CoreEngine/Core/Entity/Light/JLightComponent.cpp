#include "JLightComponent.h"

#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Interface/JWorld.h"

JLightComponent::JLightComponent()
{
	mObjectType = NAME_OBJECT_LIGHT_COMPONENT;
	JLightComponent::Initialize();
}

JLightComponent::JLightComponent(JTextView InName, AActor* InOwnerActor, JSceneComponent* InParentComponent)
	: JSceneComponent(InName, InOwnerActor, InParentComponent)
{
	mObjectType = NAME_OBJECT_LIGHT_COMPONENT;
}

void JLightComponent::Initialize()
{}

void JLightComponent::Tick(float DeltaTime)
{
	JSceneComponent::Tick(DeltaTime);;
}

uint32_t JLightComponent::GetType() const
{
	return JSceneComponent::GetType();
}

void JLightComponent::ShowEditor()
{
	JSceneComponent::ShowEditor();

	ImGui::SeparatorText(u8("라이트 설정"));
}
