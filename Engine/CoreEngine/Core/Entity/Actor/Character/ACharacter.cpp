#include "ACharacter.h"

#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"
#include "Core/Input/XKeyboardMouse.h"
#include "Core/Interface/JWorld.h"
#include "Game/Src/Player/JPlayerCamera.h"

ACharacter::ACharacter()
{
	mObjectType = NAME_OBJECT_CHARACTER;
}

ACharacter::ACharacter(JTextView InName)
	: AActor(InName),
	  mSkeletalMeshComponent(nullptr),
	  mFPSCamera(nullptr)
{
	mObjectType = NAME_OBJECT_CHARACTER;
}

void ACharacter::Initialize()
{
	AActor::Initialize();
}

void ACharacter::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);

	if (mFPSCamera)
	{
		mLocalRotation.y = XMConvertToDegrees(mFPSCamera->GetYaw());
	}
}

void ACharacter::Destroy()
{
	AActor::Destroy();
}

uint32_t ACharacter::GetType() const
{
	return HASH_ASSET_TYPE_Character;
}

bool ACharacter::Serialize_Implement(std::ofstream& FileStream)
{
	if (!AActor::Serialize_Implement(FileStream))
	{
		return false;
	}


	return true;
}

bool ACharacter::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!AActor::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	mSkeletalMeshComponent = static_cast<JSkeletalMeshComponent*>(GetChildSceneComponentByType("SkeletalMeshComponent"));
	mFPSCamera             = static_cast<JPlayerCamera*>(GetChildSceneComponentByType("CameraComponent"));

	return true;
}

void ACharacter::ShowEditor()
{
	AActor::ShowEditor();


}

FVector2 ACharacter::GetMouseDelta() const
{
	return mInput->GetCurMouseDelta();
}
