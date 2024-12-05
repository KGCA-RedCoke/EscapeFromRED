#include "JLight_Spot.h"

#include "Core/Interface/JWorld.h"

JLight_Spot::JLight_Spot()
{
	mObjectType = NAME_OBJECT_LIGHT_SPOT;
	JLight_Spot::Initialize();
}

JLight_Spot::JLight_Spot(JTextView InName, AActor* InOwnerActor, JSceneComponent* InParentComponent)
	: JLightComponent(InName, InOwnerActor, InParentComponent)
{
	mObjectType = NAME_OBJECT_LIGHT_SPOT;
	JLight_Spot::Initialize();
}

void JLight_Spot::Initialize()
{
	JLightComponent::Initialize();

	mWorldBufferIndex                                   = GetWorld.WorldSpotLightData.Count++;
	GetWorld.WorldSpotLightData.Data[mWorldBufferIndex] = mLightData;
}

void JLight_Spot::Tick(float DeltaTime)
{
	JLightComponent::Tick(DeltaTime);

	mLightData.Position = mWorldLocation;

	GetWorld.WorldSpotLightData.Data[mWorldBufferIndex] = mLightData;
}

void JLight_Spot::Destroy()
{
	JLightComponent::Destroy();

	mWorldBufferIndex = -1;
	GetWorld.WorldSpotLightData.Count--;
}

uint32_t JLight_Spot::GetType() const
{
	return JLightComponent::GetType();
}

bool JLight_Spot::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JSceneComponent::Serialize_Implement(FileStream))
	{
		return false;
	}

	// Light Data
	Utils::Serialization::Serialize_Primitive(&mLightData, sizeof(mLightData), FileStream);

	return true;
}

bool JLight_Spot::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!JSceneComponent::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	// Light Data
	Utils::Serialization::DeSerialize_Primitive(&mLightData, sizeof(mLightData), InFileStream);

	return true;
}

void JLight_Spot::ShowEditor()
{
	JLightComponent::ShowEditor();

	ImGui::SeparatorText(u8("라이트 설정"));
	ImGui::DragFloat3(u8("방향"), &mLightData.Direction.x, 0.1f);
	ImGui::ColorEdit3(u8("색상"), &mLightData.Color.x);
	ImGui::DragFloat(u8("강도"), &mLightData.Intensity, 0.1f);
	ImGui::DragFloat(u8("각도"), &mLightData.Angle, 0.1f);
	ImGui::DragFloat(u8("반경"), &mLightData.Range, 0.1f);
}
