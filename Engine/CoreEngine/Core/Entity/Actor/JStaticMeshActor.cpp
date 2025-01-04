#include "JStaticMeshActor.h"

#include "Core/Entity/Component/Mesh/JStaticMeshComponent.h"
#include "Core/Graphics/Mesh/JMeshObject.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Interface/JWorld.h"

JStaticMeshActor::JStaticMeshActor()
{
	mObjectType = NAME_OBJECT_STATIC_MESH_ACTOR;
}

JStaticMeshActor::JStaticMeshActor(JTextView InName, JTextView SavedMeshPath)
	: AActor(InName)
{
	mObjectType = NAME_OBJECT_STATIC_MESH_ACTOR;

	CreateMeshComponent(SavedMeshPath);

}

JStaticMeshActor::JStaticMeshActor(const JStaticMeshActor& Copy)
	: AActor(Copy)
{
	CreateMeshComponent(Copy.mStaticMeshComponent->GetName());
}

UPtr<IManagedInterface> JStaticMeshActor::Clone() const
{
	return MakeUPtr<JStaticMeshActor>(*this);
}


void JStaticMeshActor::Initialize()
{
	if (bUseMeshCollision)
	{
		// CreateDefaultSubObject<JBoxComponent>("BoxCollision", this, this);
		// mStaticMeshComponent->SetCollisionType(ECollisionType::Box);
		// mStaticMeshComponent->SetCollisionEnabled(true);
	}

	AActor::Initialize();

	AActor::Tick(0);
	mBoundingBox = mStaticMeshComponent->GetBoundingVolume();
}

void JStaticMeshActor::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);
}

bool JStaticMeshActor::Serialize_Implement(std::ofstream& FileStream)
{
	if (!AActor::Serialize_Implement(FileStream))
	{
		return false;
	}

	// Chunk Data
	Utils::Serialization::Serialize_Primitive(&mMeshChunkData, sizeof(mMeshChunkData), FileStream);


	return true;
}

bool JStaticMeshActor::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!AActor::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	mStaticMeshComponent = static_cast<JStaticMeshComponent*>(GetChildSceneComponentByType("JStaticMeshComponent"));

	int32_t      currentFilePos = InFileStream.tellg();
	JAssetHeader header;
	Utils::Serialization::DeserializeHeader(InFileStream, header);

	// 원래 위치로 이동
	InFileStream.seekg(currentFilePos);

	if (header.Version == 2)
	{
		// Chunk Data
		Utils::Serialization::DeSerialize_Primitive(&mMeshChunkData, sizeof(mMeshChunkData), InFileStream);
		if (mMeshChunkData.bChunkMesh && mStaticMeshComponent)
		{
			FMatrix startMat = XMMatrixTranslation(mWorldLocation.x, mWorldLocation.y, mWorldLocation.z);
			mStaticMeshComponent->GetMeshObject()->GenerateRandomInstanceData(mMeshChunkData.mChunkSize,
																			  startMat,
																			  mMeshChunkData.AreaSize,
																			  mMeshChunkData.NoiseStrength);
		}
	}

	return true;
}

void JStaticMeshActor::ShowEditor()
{
	AActor::ShowEditor();

	ImGui::Checkbox(u8("메시 그대로 콜리전 사용"), &bUseMeshCollision);

	AActor::Tick(0);
	mBoundingBox = mStaticMeshComponent->GetBoundingVolume();

	const bool prevState = mMeshChunkData.bChunkMesh;
	ImGui::Checkbox(u8("대량 메시 렌더"), &mMeshChunkData.bChunkMesh);
	if (prevState != mMeshChunkData.bChunkMesh)
	{
		mStaticMeshComponent->GetMeshObject()->bChunkMesh = mMeshChunkData.bChunkMesh;
	}

	if (mMeshChunkData.bChunkMesh)
	{
		// 인스턴스 크기
		ImGui::DragInt(u8("갯수"), &mMeshChunkData.mChunkSize, 1, 1, 5000);

		// 영역 크기
		ImGui::DragFloat(u8("영역 크기"), &mMeshChunkData.AreaSize, 1.f, 100.f, 10000.f);

		// 노이즈 강도
		ImGui::DragFloat(u8("노이즈 강도"), &mMeshChunkData.NoiseStrength, 1.f, 0.01f, 1000.f);

		// 새로고침 버튼
		if (ImGui::Button(u8("새로고침")) || prevState != mMeshChunkData.bChunkMesh)
		{
			mStaticMeshComponent->GetMeshObject()->GenerateRandomInstanceData(mMeshChunkData.mChunkSize,
																			  mWorldMat,
																			  mMeshChunkData.AreaSize,
																			  mMeshChunkData.NoiseStrength);
		}
	}
}

int32_t JStaticMeshActor::GetMaterialCount() const
{
	if (!mStaticMeshComponent)
	{
		LOG_CORE_ERROR("유효하지 않은 메시 오브젝트.");
		return 0;
	}

	return mStaticMeshComponent->GetMaterialCount();
}

void JStaticMeshActor::SetMaterialInstance(JMaterialInstance* InMaterialInstance, uint32_t InIndex)
{
	if (!mStaticMeshComponent)
	{
		LOG_CORE_ERROR("유효하지 않은 메시 오브젝트.");
		return;
	}

	mStaticMeshComponent->SetMaterialInstance(InMaterialInstance, InIndex);
}

void JStaticMeshActor::CreateMeshComponent(JTextView InMeshObject)
{
	// 메시 할당
	mStaticMeshComponent = CreateDefaultSubObject<JStaticMeshComponent>(InMeshObject, this, this);
	mStaticMeshComponent->SetMeshObject(InMeshObject);
	mBoundingBox = mStaticMeshComponent->GetBoundingVolume();

	// 메시 컴포넌트 부착 (루트에 부착)
	// mStaticMeshComponent->SetupAttachment(mRootComponent);
}
