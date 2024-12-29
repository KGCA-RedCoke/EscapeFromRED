#include "JSkeletalMeshObject.h"

#include "MMeshManager.h"
#include "Core/Entity/Animation/MAnimManager.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/JWorld.h"

JSkeletalMeshObject::JSkeletalMeshObject(const JText& InName, const JArray<Ptr<JMeshData>>& InData)
{
	mName = InName;

	if (InData.empty())
	{
		if (!Utils::Serialization::DeSerialize(InName.c_str(), this))
		{
			LOG_CORE_ERROR("Failed to load mesh object(Invalid Path maybe...): {0}", InName);
		}
	}
	else
	{
		mPrimitiveMeshData = InData;
	}

	assert(mPrimitiveMeshData.size() > 0);

	mSkeletalMesh = dynamic_cast<JSkeletalMesh*>(mPrimitiveMeshData[0].get());
}

JSkeletalMeshObject::JSkeletalMeshObject(const JSkeletalMeshObject& Other)
{
	mName              = Other.mName;
	mGeometryBuffer    = Other.mGeometryBuffer;
	mVertexSize        = Other.mVertexSize;
	mBoundingBox       = Other.mBoundingBox;
	mPath              = Other.mPath;
	mPrimitiveMeshData = Other.mPrimitiveMeshData;
	mSkeletalMesh      = Other.mSkeletalMesh;

	mInstanceData.resize(Other.mInstanceData.size());
	mMaterialInstances.resize(Other.mMaterialInstances.size());

	for (int32_t i = 0; i < Other.mInstanceData.size(); ++i)
	{
		JSkeletalMeshObject::SetMaterialInstance(Other.mMaterialInstances[i], i);
	}
}

JSkeletalMeshObject::~JSkeletalMeshObject()
{
	mSkeletalMesh = nullptr;
	mPrimitiveMeshData.clear();
}

UPtr<IManagedInterface> JSkeletalMeshObject::Clone() const
{
	return MakeUPtr<JSkeletalMeshObject>(*this);
}

const FMatrix& JSkeletalMeshObject::GetAnimBoneMatrix(const JText& Text) const
{
	if (mCurrentAnimation)
	{
		return mCurrentAnimation->GetInterpolatedBone(Text);
	}
	return FMatrix::Identity;
}

uint32_t JSkeletalMeshObject::GetType() const
{
	return HASH_ASSET_TYPE_SKELETAL_MESH;
}

bool JSkeletalMeshObject::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JMeshObject::Serialize_Implement(FileStream))
	{
		return false;
	}

	return true;
}

bool JSkeletalMeshObject::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, this->GetType()))
	{
		return false;
	}

	// Mesh Name
	Utils::Serialization::DeSerialize_Text(mName, InFileStream);

	// Primitive Mesh Data
	int32_t meshDataSize;
	Utils::Serialization::DeSerialize_Primitive(&meshDataSize, sizeof(meshDataSize), InFileStream);

	for (int32_t i = 0; i < meshDataSize; ++i)
	{
		auto archivedData = MakePtr<JSkeletalMesh>();
		archivedData->DeSerialize_Implement(InFileStream);
		mPrimitiveMeshData.push_back(archivedData);
	}

	mMaterialInstances.resize(mPrimitiveMeshData[0]->GetSubMaterialNum());
	mInstanceData.resize(mPrimitiveMeshData[0]->GetSubMaterialNum());
	for (int32_t i = 0; i < mPrimitiveMeshData[0]->GetSubMaterialNum(); ++i)
	{
		JText materialPath;
		Utils::Serialization::DeSerialize_Text(materialPath, InFileStream);
		auto* matInstance = GetWorld.MaterialInstanceManager->Load(materialPath);

		SetMaterialInstance(matInstance, i);
	}

	// Bounding Box
	Utils::Serialization::DeSerialize_Primitive(&mBoundingBox, sizeof(mBoundingBox), InFileStream);

	return true;
}

void JSkeletalMeshObject::Tick(float DeltaTime)
{
	if (mCurrentAnimation)
	{
		if (mCurrentAnimation->TickAnim(DeltaTime))
		{

			for (int32_t i = 0; i < mInstanceData.size(); ++i)
			{
				mInstanceData[i].SkeletalData = mCurrentAnimation->GetInstanceData();
			}

		}
		if (bTransitAnimation)
		{
			auto& animData = mCurrentAnimation->GetInstanceData();

			mTransitionElapsedTime += DeltaTime;

			float BlendWeight = std::clamp(mTransitionElapsedTime / 0.2f, 0.0f, 1.0f);

			for (int32_t i = 0; i < mInstanceData.size(); ++i)
			{
				mInstanceData[i].SkeletalData.BlendWeight    = BlendWeight;
				mInstanceData[i].SkeletalData.NextAnimOffset = animData.CurrentAnimOffset;
				mInstanceData[i].SkeletalData.NextAnimIndex  = animData.CurrentAnimIndex;
			}

			// 블렌딩 완료 시
			if (BlendWeight >= 1.0f)
			{
				bTransitAnimation      = false;
				mTransitionElapsedTime = 0.0f;
				mPreviewAnimationClip  = nullptr;
			}
		}

	}
}

void JSkeletalMeshObject::AddInstance(float InCameraDistance)
{
	auto&                      meshData     = mPrimitiveMeshData[0];
	auto&                      subMeshes    = meshData->GetSubMesh();
	const int32_t              subMeshCount = subMeshes.empty() ? 1 : subMeshes.size();
	auto                       it           = GetWorld.AnimationManager->mAnimTextureBuffer_SRV.find(mSkeletalMesh);
	ID3D11ShaderResourceView** srv          = nullptr;
	if (it != GetWorld.AnimationManager->mAnimTextureBuffer_SRV.end())
	{
		srv = it->second.GetAddressOf();
	}

	for (int32_t j = 0; j < subMeshCount; ++j)
	{
		auto& currMesh = subMeshes.empty() ? meshData : subMeshes[j];
		GetWorld.MeshManager->PushCommand(currMesh->GetHash(),
										  mMaterialInstances[j],
										  mInstanceData[j],
										  srv);
	}

}

void JSkeletalMeshObject::Draw()
{
	auto* deviceContext = GetWorld.D3D11API->GetImmediateDeviceContext();
	assert(deviceContext);

	auto&         meshData     = mPrimitiveMeshData[0];
	auto&         subMeshes    = meshData->GetSubMesh();
	const int32_t subMeshCount = subMeshes.empty() ? 1 : subMeshes.size();

	auto                       it  = GetWorld.AnimationManager->mAnimTextureBuffer_SRV.find(mSkeletalMesh);
	ID3D11ShaderResourceView** srv = nullptr;
	if (it != GetWorld.AnimationManager->mAnimTextureBuffer_SRV.end())
	{
		srv = it->second.GetAddressOf();
	}

	for (int32_t j = 0; j < subMeshCount; ++j)
	{
		auto& currMesh = subMeshes.empty() ? meshData : subMeshes[j];
		GetWorld.MeshManager->PushCommand(currMesh->GetHash(),
										  mMaterialInstances[j],
										  mInstanceData[j],
										  srv);
	}

	GetWorld.MeshManager->FlushCommandList(G_Device.GetImmediateDeviceContext());
}

void JSkeletalMeshObject::DrawID(uint32_t ID)
{
	JMeshObject::DrawID(ID);
}

void JSkeletalMeshObject::UpdateInstance_Anim(const FSkeletalMeshInstanceData& InData)
{
	for (int32_t i = 0; i < mInstanceData.size(); ++i)
	{
		mInstanceData[i].SkeletalData = InData;
	}
}

void JSkeletalMeshObject::SetAnimation(JAnimationClip* InAnimation)
{
	if (mCurrentAnimation)
	{
		mCurrentAnimation->Stop();
		mPreviewAnimationClip = mCurrentAnimation;
		bTransitAnimation     = true;
	}

	mCurrentAnimation = InAnimation;
	if (!mCurrentAnimation->GetSkeletalMesh())
	{
		mCurrentAnimation->SetSkeletalMesh(mSkeletalMesh);
	}
	mCurrentAnimation->Play();
}
