#include "JLight_Point.h"

#include "Core/Interface/JWorld.h"

JLight_Point::JLight_Point()
{
	mObjectType = NAME_OBJECT_LIGHT_POINT;
	JLight_Point::Initialize();
}

JLight_Point::JLight_Point(JTextView InName, AActor* InOwnerActor, JSceneComponent* InParentComponent)
	: JLightComponent(InName, InOwnerActor, InParentComponent)
{
	mObjectType = NAME_OBJECT_LIGHT_POINT;
	JLight_Point::Initialize();
}

void JLight_Point::Initialize()
{
	mWorldBufferIndex                                    = GetWorld.WorldPointLightData.Count++;
	GetWorld.WorldPointLightData.Data[mWorldBufferIndex] = mLightData;
}

void JLight_Point::Tick(float DeltaTime)
{
	JLightComponent::Tick(DeltaTime);

	mLightData.Position = mWorldLocation;

	GetWorld.WorldPointLightData.Data[mWorldBufferIndex] = mLightData;
}

uint32_t JLight_Point::GetType() const
{
	return JLightComponent::GetType();
}

bool JLight_Point::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JSceneComponent::Serialize_Implement(FileStream))
	{
		return false;
	}

	// Light Data
	Utils::Serialization::Serialize_Primitive(&mLightData, sizeof(mLightData), FileStream);

	return true;
}

bool JLight_Point::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!JSceneComponent::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	// Light Data
	Utils::Serialization::DeSerialize_Primitive(&mLightData, sizeof(mLightData), InFileStream);

	return true;
}

void JLight_Point::ShowEditor()
{
	JSceneComponent::ShowEditor();

	ImGui::SeparatorText(u8("라이트 설정"));
	ImGui::ColorEdit3(u8("색상"), &mLightData.Color.x);
	ImGui::DragFloat(u8("강도"), &mLightData.Intensity, 0.1f);
	ImGui::DragFloat(u8("반경"), &mLightData.Range, 0.1f);
}
