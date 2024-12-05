#include "JLightComponent.h"

#include "Core/Graphics/Shader/JShader_Basic.h"
#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Interface/JWorld.h"

JLightComponent::JLightComponent()
	: mLightData()
{
	JLightComponent::Initialize();
}

JLightComponent::JLightComponent(JTextView InName, AActor* InOwnerActor, JSceneComponent* InParentComponent)
	: JSceneComponent(InName, InOwnerActor, InParentComponent),
	  mLightData()
{
	JLightComponent::Initialize();
}

void JLightComponent::Initialize()
{
	mWorldBufferIndex                                    = GetWorld.WorldPointLightData.Count++;
	GetWorld.WorldPointLightData.Data[mWorldBufferIndex] = mLightData;

}

void JLightComponent::Tick(float DeltaTime)
{
	JSceneComponent::Tick(DeltaTime);

	GetWorld.WorldPointLightData.Data[mWorldBufferIndex] = mLightData;
}

uint32_t JLightComponent::GetType() const
{
	return JSceneComponent::GetType();
}

bool JLightComponent::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JSceneComponent::Serialize_Implement(FileStream))
	{
		return false;
	}

	// Light Data
	Utils::Serialization::Serialize_Primitive(&mLightData, sizeof(mLightData), FileStream);

	return true;
}

bool JLightComponent::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!JSceneComponent::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	// Light Data
	Utils::Serialization::DeSerialize_Primitive(&mLightData, sizeof(mLightData), InFileStream);

	return true;
}

void JLightComponent::ShowEditor()
{
	JSceneComponent::ShowEditor();

	ImGui::SeparatorText(u8("라이트 설정"));
	ImGui::DragFloat3(u8("위치"), &mLightData.Position.x, 0.1f);
	ImGui::ColorEdit3(u8("색상"), &mLightData.Color.x);
	ImGui::DragFloat(u8("강도"), &mLightData.Intensity, 0.1f);
	ImGui::DragFloat(u8("반경"), &mLightData.Range, 0.1f);
}
