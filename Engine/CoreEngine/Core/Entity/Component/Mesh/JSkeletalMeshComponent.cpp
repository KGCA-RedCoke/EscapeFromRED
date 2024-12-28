#include "JSkeletalMeshComponent.h"

#include "Core/Entity/Animation/JAnimator.h"
#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Graphics/Mesh/JSkeletalMeshObject.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Interface/JWorld.h"

JSkeletalMeshComponent::JSkeletalMeshComponent()
{
	mObjectType = NAME_OBJECT_SKELETAL_MESH_COMPONENT;
}

JSkeletalMeshComponent::JSkeletalMeshComponent(JTextView InName, AActor* InOwnerActor, JSceneComponent* InParentComponent)
	: JSceneComponent(InName, InOwnerActor, InParentComponent)
{
	mObjectType = NAME_OBJECT_SKELETAL_MESH_COMPONENT;
}

uint32_t JSkeletalMeshComponent::GetType() const
{
	return StringHash("SkeletonMeshComponent");
}

bool JSkeletalMeshComponent::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JSceneComponent::Serialize_Implement(FileStream))
	{
		return false;
	}

	bool bHasMeshObject = mSkeletalMeshObject != nullptr;
	Utils::Serialization::Serialize_Primitive(&bHasMeshObject, sizeof(bool), FileStream);
	if (bHasMeshObject)
	{
		auto filePath = std::format("Game/Mesh/{}.jasset", mSkeletalMeshObject->GetName());
		Utils::Serialization::Serialize_Text(filePath, FileStream);
	}

	return true;
}

bool JSkeletalMeshComponent::DeSerialize_Implement(std::ifstream& InFileStream)
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

		SetSkeletalMesh(filePath);
	}

	return true;
}

void JSkeletalMeshComponent::Initialize()
{
	JSceneComponent::Initialize();

}

void JSkeletalMeshComponent::Tick(float DeltaTime)
{
	// TransformComponent에서 위치 업데이트
	JSceneComponent::Tick(DeltaTime);

	// 프러스텀 박스(OBB) 업데이트
	mBoundingBox.Box.LocalAxis[0] = XMVector3TransformNormal(FVector(1, 0, 0), XMLoadFloat4x4(&mWorldMat));
	mBoundingBox.Box.LocalAxis[1] = XMVector3TransformNormal(FVector(0, 1, 0), XMLoadFloat4x4(&mWorldMat));
	mBoundingBox.Box.LocalAxis[2] = XMVector3TransformNormal(FVector(0, 0, 1), XMLoadFloat4x4(&mWorldMat));
	mBoundingBox.Box.Center       = XMVector3Transform(0.5f * (mBoundingBox.Max + mBoundingBox.Min), mWorldMat);
	mBoundingBox.Box.Extent       = 0.5f * (mBoundingBox.Max - mBoundingBox.Min);

	// MeshObject의 버퍼 업데이트
	if (mSkeletalMeshObject)
	{
		if (mAnimator)
		{
			mAnimator->Tick(DeltaTime);
		}

		mSkeletalMeshObject->Tick(DeltaTime);
		mSkeletalMeshObject->UpdateInstance_Transform(mWorldMat);
	}
}

void JSkeletalMeshComponent::Draw()
{
	// MeshObject의 Draw 호출
	if (mSkeletalMeshObject)
	{
		FVector distance = mWorldLocation - GetWorld.CameraManager->GetCurrentMainCam()->GetWorldLocation();
		float   dist     = distance.Length();
		// mBoundingBox.DrawDebug();
		mSkeletalMeshObject->AddInstance(dist);
	}

	JSceneComponent::Draw();
}

void JSkeletalMeshComponent::DrawID(uint32_t ID)
{
	if (mSkeletalMeshObject)
	{
		mSkeletalMeshObject->DrawID(ID);
	}

	JSceneComponent::DrawID(ID);
}

Ptr<JSkeletalMesh> JSkeletalMeshComponent::GetSkeletalMesh() const
{
	return mSkeletalMeshObject ? mSkeletalMeshObject->GetSkeletalMesh() : nullptr;
}

void JSkeletalMeshComponent::SetSkeletalMesh(JTextView InSkeletalMeshPath)
{
	mSkeletalMeshObject = GetWorld.MeshManager->Clone<JSkeletalMeshObject>(InSkeletalMeshPath.data());
	mBoundingBox        = mSkeletalMeshObject->GetBoundingBox();
}

void JSkeletalMeshComponent::SetMaterialInstance(JMaterialInstance* InMaterialInstance, uint32_t InSlot) const
{
	if (mSkeletalMeshObject)
	{
		mSkeletalMeshObject->SetMaterialInstance(InMaterialInstance, InSlot);
	}
}

void JSkeletalMeshComponent::SetAnimation(JAnimationClip* InAnimationClip) const
{
	if (mSkeletalMeshObject)
	{
		mSkeletalMeshObject->SetAnimation(InAnimationClip);
	}
}

void JSkeletalMeshComponent::ShowEditor()
{
	JSceneComponent::ShowEditor();

	ImGui::SeparatorText(u8("스켈레탈 메시"));

	ImGui::Image(nullptr, ImVec2(100, 100));
	if (ImGui::IsMouseReleased(0) && ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::GetDragDropPayload();;
		{
			const char* assetPath = static_cast<const char*>(payload->Data);
			auto        metaData  = Utils::Serialization::GetType(assetPath);

			if (metaData.AssetType == HASH_ASSET_TYPE_SKELETAL_MESH)
			{
				SetSkeletalMesh(assetPath);
			}

		}
	}

	if (!mSkeletalMeshObject)
	{
		return;
	}

	ImGui::SeparatorText(u8("머티리얼 슬롯"));
	for (int32_t i = 0; i < mSkeletalMeshObject->GetMaterialCount(); ++i)
	{
		const JMaterialInstance* materialInstance = mSkeletalMeshObject->GetMaterialInstance(i);
		ImGui::Text(materialInstance->GetMaterialName().c_str());
		ImGui::Image(nullptr, ImVec2(100, 100));
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

	ImGui::SeparatorText(u8("애니메이션"));
	ImGui::Image(nullptr, ImVec2(100, 100));
	// ImGui::Text(subMeshes[j]->GetName().c_str());

	if (ImGui::IsMouseReleased(0) && ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::GetDragDropPayload();;
		const char*         str     = static_cast<const char*>(payload->Data);

		auto metaData = Utils::Serialization::GetType(str);

		switch (metaData.AssetType)
		{
		case HASH_ASSET_TYPE_ANIMATION_CLIP:
			break;
		// case HASH_ASSET_TYPE_ANIMATOR:
		// 	break;
		}

		if (metaData.AssetType == HASH_ASSET_TYPE_ANIMATION_CLIP)
		{
			// mPreviewAnimationClip = GetWorld.AnimationManager->Clone(str);

			mSkeletalMeshObject->
					SetAnimation(GetWorld.AnimationManager->Load(str, mSkeletalMeshObject->GetSkeletalMesh()));

			ImGui::EndDragDropTarget();
		}
	}

}

const FMatrix& JSkeletalMeshComponent::GetAnimBoneMatrix(const JText& Text) const
{
	if (mSkeletalMeshObject)
	{
		return mSkeletalMeshObject->GetAnimBoneMatrix(Text);
	}

	return FMatrix::Identity;
}
