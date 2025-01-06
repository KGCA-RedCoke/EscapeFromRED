#include "JBillboardComponent.h"

#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Graphics/Mesh/JMeshObject.h"
#include "Core/Interface/JWorld.h"

JBillboardComponent::JBillboardComponent()
{
	SetMeshObject("Game/Mesh/Plane.jasset");
}

JBillboardComponent::JBillboardComponent(JTextView InName, AActor* InOwnerActor, JSceneComponent* InParentSceneComponent)
{
	SetMeshObject("Game/Mesh/Plane.jasset");
}

JBillboardComponent::JBillboardComponent(const JBillboardComponent& Copy)
	: JStaticMeshComponent(Copy) {}

JBillboardComponent::~JBillboardComponent() {}

bool JBillboardComponent::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JSceneComponent::Serialize_Implement(FileStream))
	{
		return false;
	}

	auto* materialInstance = mMeshObject->GetMaterialInstance(0);
	auto  materialPath     = materialInstance->GetMaterialPath();
	Utils::Serialization::Serialize_Text(materialPath, FileStream);

	return true;
}

bool JBillboardComponent::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!JSceneComponent::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	JText materialPath;
	Utils::Serialization::DeSerialize_Text(materialPath, InFileStream);
	auto materialInstance = MMaterialInstanceManager::Get().Load(materialPath);
	SetMaterialInstance(materialInstance, 0);

	return true;
}

void JBillboardComponent::Initialize()
{
	JStaticMeshComponent::Initialize();
}

void JBillboardComponent::Tick(float DeltaTime)
{
	// 현재 카메라 룩 벡터를 얻어온다.
	const auto& camera = GetWorld.CameraManager->GetCurrentMainCam();

	// 빌보드는 카메라의 위치를 바라보도록 한다.
	FVector objectToCamera = camera->GetWorldLocation() - mWorldLocation;
	objectToCamera.Normalize();

	FVector upVector = FVector(0, 1, 0); // 월드 Up 벡터 (Y축)

	FVector rightVector = upVector.Cross(objectToCamera); // X축
	FVector correctedUp = objectToCamera.Cross(rightVector); // Y축

	rightVector.Normalize();
	correctedUp.Normalize();

	// 객체의 월드 회전 행렬
	FMatrix rotationMatrix = {
		rightVector.x, rightVector.y, rightVector.z, 0,
		correctedUp.x, correctedUp.y, correctedUp.z, 0,
		objectToCamera.x, objectToCamera.y, objectToCamera.z, 0,
		0, 0, 0, 1
	};

	mLocalRotation = XMQuaternionRotationMatrix(rotationMatrix);
	mLocalLocation.ConvertToDegrees();

	JStaticMeshComponent::Tick(DeltaTime);

	mMeshObject->UpdateInstance_Transform(mWorldMat);
}

void JBillboardComponent::ShowEditor()
{
	JSceneComponent::ShowEditor();

	ImGui::SeparatorText(u8("텍스처 슬롯"));
	for (int32_t i = 0; i < GetMaterialCount(); ++i)
	{
		const JMaterialInstance* materialInstance = mMeshObject->GetMaterialInstance(i);
		ImGui::Text(materialInstance->GetMaterialName().c_str());
		ImGui::Dummy(ImVec2(100, 100));
		if (ImGui::IsMouseReleased(0) && ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::GetDragDropPayload();;
			const char*         str     = static_cast<const char*>(payload->Data);

			auto metaData = Utils::Serialization::GetType(str);

			if (metaData.AssetType != HASH_ASSET_TYPE_MATERIAL_INSTANCE)
			{
				return;
			}

			if (auto matInstancePtr = MMaterialInstanceManager::Get().Load(str))
			{
				SetMaterialInstance(matInstancePtr, i);
				return;
			}

			ImGui::EndDragDropTarget();
		}
	}
}
