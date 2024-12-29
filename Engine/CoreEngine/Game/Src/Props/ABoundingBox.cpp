#include "ABoundingBox.h"

ABoundingBox::ABoundingBox()
	: AActor()
{
	mObjectType = NAME_OBJECT_BOUNDING_BOX;
}

ABoundingBox::ABoundingBox(JTextView InName)
	: AActor(InName)
{
	mObjectType = NAME_OBJECT_BOUNDING_BOX;
}

uint32_t ABoundingBox::GetType() const
{
	return HASH_ASSET_TYPE_BoundingBox;
}

bool ABoundingBox::Serialize_Implement(std::ofstream& FileStream)
{
	return AActor::Serialize_Implement(FileStream);
}

bool ABoundingBox::DeSerialize_Implement(std::ifstream& InFileStream)
{
	bool bSuccess = AActor::DeSerialize_Implement(InFileStream);

	mBoxComponent = static_cast<JBoxComponent*>(GetChildComponentByType(NAME_COMPONENT_BOX));

	return bSuccess;
}

void ABoundingBox::Initialize()
{
	AActor::Initialize();

	if (!mBoxComponent)
	{
		mBoxComponent = CreateDefaultSubObject<JBoxComponent>("BoxComponent", this);
	}
}

void ABoundingBox::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);
}

void ABoundingBox::Destroy()
{
	AActor::Destroy();
}

void ABoundingBox::ShowEditor()
{
	AActor::ShowEditor();

	ImGui::SeparatorText("BoxComponent");

	mBoxComponent->ShowEditor();
}
