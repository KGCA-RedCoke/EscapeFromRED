#include "JStaticMeshComponent.h"

#include "JSkeletalMeshComponent.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Graphics/Mesh/JMeshObject.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Interface/JWorld.h"

JStaticMeshComponent::JStaticMeshComponent()
{
	mObjectType = NAME_OBJECT_STATIC_MESH_COMPONENT;
}

JStaticMeshComponent::JStaticMeshComponent(JTextView        InName,
										   AActor*          InOwnerActor,
										   JSceneComponent* InParentSceneComponent)
	: JSceneComponent(InName, InOwnerActor, InParentSceneComponent)
{
	mObjectType = NAME_OBJECT_STATIC_MESH_COMPONENT;
}

JStaticMeshComponent::JStaticMeshComponent(const JStaticMeshComponent& Copy)
	: JSceneComponent(Copy)
{
	mBoundingBox = Copy.mBoundingBox;
	mMeshObject  = UPtrCast<JMeshObject>(Copy.mMeshObject->Clone());
}

JStaticMeshComponent::~JStaticMeshComponent() {}

bool JStaticMeshComponent::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JSceneComponent::Serialize_Implement(FileStream))
	{
		return false;
	}

	bool bHasMeshObject = mMeshObject != nullptr;
	Utils::Serialization::Serialize_Primitive(&bHasMeshObject, sizeof(bool), FileStream);
	if (bHasMeshObject)
	{
		// mMeshObject->Serialize_Implement(FileStream);
		auto filePath = std::format("Game/Mesh/{}.jasset", mMeshObject->GetName());
		Utils::Serialization::Serialize_Text(filePath, FileStream);
	}

	// MaterialInstance
	int32_t materialCount = mMeshObject->GetMaterialCount();
	Utils::Serialization::Serialize_Primitive(&materialCount, sizeof(int32_t), FileStream);
	for (int32_t i = 0; i < materialCount; ++i)
	{
		auto* materialInstance = mMeshObject->GetMaterialInstance(i);
		auto  materialPath     = materialInstance->GetMaterialPath();
		Utils::Serialization::Serialize_Text(materialPath, FileStream);
	}

	return true;
}

bool JStaticMeshComponent::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!JSceneComponent::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	bool bHasMeshObject;
	Utils::Serialization::DeSerialize_Primitive(&bHasMeshObject, sizeof(bool), InFileStream);

	if (bHasMeshObject)
	{
		JText filePath;
		Utils::Serialization::DeSerialize_Text(filePath, InFileStream);

		SetMeshObject(filePath);
	}

	int32_t materialCount;
	Utils::Serialization::DeSerialize_Primitive(&materialCount, sizeof(int32_t), InFileStream);
	for (int32_t i = 0; i < materialCount; ++i)
	{
		JText materialPath;
		Utils::Serialization::DeSerialize_Text(materialPath, InFileStream);

		auto materialInstance = MMaterialInstanceManager::Get().Load(materialPath);
		SetMaterialInstance(materialInstance, i);
	}

	return true;
}


void JStaticMeshComponent::Tick(float DeltaTime)
{
	// TransformComponent에서 위치 업데이트
	JSceneComponent::Tick(DeltaTime);

	// Step1.1 프러스텀 박스(OBB) 업데이트
	mBoundingBox.Box.LocalAxis[0] = XMVector3TransformNormal(FVector(1, 0, 0), XMLoadFloat4x4(&mWorldMat));
	mBoundingBox.Box.LocalAxis[1] = XMVector3TransformNormal(FVector(0, 1, 0), XMLoadFloat4x4(&mWorldMat));
	mBoundingBox.Box.LocalAxis[2] = XMVector3TransformNormal(FVector(0, 0, 1), XMLoadFloat4x4(&mWorldMat));

	mBoundingBox.Box.Center = 0.5f * (mBoundingBox.Max + mBoundingBox.Min);
	mBoundingBox.Box.Center = XMVector3Transform(mBoundingBox.Box.Center, mWorldMat);
	mBoundingBox.Box.Extent = 0.5f * (mBoundingBox.Max - mBoundingBox.Min);

	// MeshObject의 버퍼 업데이트
	if (mMeshObject)
	{
		mMeshObject->Tick(DeltaTime);
		mMeshObject->UpdateInstance_Transform(mWorldMat);
	}
}

void JStaticMeshComponent::Draw()
{
	// MeshObject의 Draw 호출
	if (mMeshObject)
	{
		mBoundingBox.DrawDebug();
		FVector distance = mWorldLocation - GetWorld.CameraManager->GetCurrentMainCam()->GetWorldLocation();
		float   dist     = distance.Length();
		mMeshObject->AddInstance(dist);
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

void JStaticMeshComponent::SetMaterialInstance(JMaterialInstance* InMaterialInstance, uint32_t InSlot)
{
	if (!mMeshObject)
	{
		LOG_CORE_ERROR("MeshObject가 없음.");
		return;
	}

	mMeshObject->SetMaterialInstance(InMaterialInstance, InSlot);
}

void JStaticMeshComponent::SetMeshObject(JTextView InMeshObject)
{
	mMeshObject  = GetWorld.MeshManager->Clone(InMeshObject.data());
	mBoundingBox = mMeshObject->GetBoundingBox();
}

int32_t JStaticMeshComponent::GetMaterialCount() const
{
	return mMeshObject ? mMeshObject->GetMaterialCount() : 0;
}

void JStaticMeshComponent::ShowEditor()
{
	JSceneComponent::ShowEditor();

	if (/*mParentSceneComponent->GetObjectType() == NAME_OBJECT_SKELETAL_MESH_COMPONENT*/
		auto* parentSkeletalMesh = dynamic_cast<JSkeletalMeshComponent*>(mParentSceneComponent))
	{
		ImGui::SeparatorText(u8("소켓"));

	}

	ImGui::SeparatorText(u8("머티리얼 슬롯"));
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
