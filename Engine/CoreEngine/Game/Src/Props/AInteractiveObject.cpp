#include "AInteractiveObject.h"

AInteractiveObject::AInteractiveObject()
	: AActor()
{
	mObjectType = NAME_OBJECT_BOUNDING_BOX;
}

AInteractiveObject::AInteractiveObject(JTextView InName)
	: AActor(InName)
{
	mObjectType = NAME_OBJECT_BOUNDING_BOX;
}

uint32_t AInteractiveObject::GetType() const
{
	return HASH_ASSET_TYPE_BoundingBox;
}

bool AInteractiveObject::Serialize_Implement(std::ofstream& FileStream)
{
	return AActor::Serialize_Implement(FileStream);
}

bool AInteractiveObject::DeSerialize_Implement(std::ifstream& InFileStream)
{
	bool bSuccess = AActor::DeSerialize_Implement(InFileStream);

	mBoxComponent = static_cast<JBoxComponent*>(GetChildComponentByType(NAME_COMPONENT_BOX));

	return bSuccess;
}

void AInteractiveObject::Initialize()
{
	AActor::Initialize();

	if (!mBoxComponent)
	{
		mBoxComponent = CreateDefaultSubObject<JBoxComponent>("BoxComponent", this);
	}
}

void AInteractiveObject::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);
}

void AInteractiveObject::Destroy()
{
	AActor::Destroy();
}

void AInteractiveObject::ShowEditor()
{
	AActor::ShowEditor();

	ImGui::SeparatorText("BoxComponent");

	mBoxComponent->ShowEditor();
}
