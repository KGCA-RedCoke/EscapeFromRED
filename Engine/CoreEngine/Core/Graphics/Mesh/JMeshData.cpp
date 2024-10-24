#include "JMeshData.h"
#include "Core/Graphics/Material/JMaterial.h"
#include "Core/Graphics/Material/Instance/JMaterialInstance.h"
#include "Core/Graphics/Texture/JTexture.h"
#include "Core/Interface/MManagerInterface.h"

Ptr<IManagedInterface> JMeshData::Clone() const
{
	Ptr<JMeshData> clonedMesh = MakePtr<JMeshData>();

	clonedMesh->mName                  = mName;
	clonedMesh->mIndex                 = mIndex;
	clonedMesh->mClassType             = mClassType;
	clonedMesh->mFaceCount             = mFaceCount;
	clonedMesh->mMaterialRefNum        = mMaterialRefNum;
	clonedMesh->mInitialModelTransform = mInitialModelTransform;

	if (mParentMesh.lock())
	{
		Ptr<JMeshData> parentMesh = mParentMesh.lock();
		clonedMesh->mParentMesh   = dynamic_pointer_cast<JMeshData>(parentMesh->Clone());
	}
	clonedMesh->mMaterialInstance = std::dynamic_pointer_cast<JMaterialInstance>(mMaterialInstance->Clone());

	clonedMesh->mSubMesh.reserve(mSubMesh.size());
	clonedMesh->mChildMesh.reserve(mChildMesh.size());

	for (int32_t i = 0; i < mSubMesh.size(); ++i)
	{
		clonedMesh->mSubMesh.push_back(std::dynamic_pointer_cast<JMeshData>(mSubMesh[i]->Clone()));
	}

	for (int32_t i = 0; i < mChildMesh.size(); ++i)
	{
		clonedMesh->mChildMesh.push_back(std::dynamic_pointer_cast<JMeshData>(mChildMesh[i]->Clone()));
	}

	clonedMesh->mVertexData = std::dynamic_pointer_cast<JVertexData<Vertex::FVertexInfo_Base>>(mVertexData->Clone());

	for (const auto& [boneName, bindPose] : mBindPoseMap)
	{
		clonedMesh->mBindPoseMap.try_emplace(boneName, bindPose);
	}

	return clonedMesh;
}

uint32_t JMeshData::GetHash() const
{
	return StringHash(mName.c_str());
}

uint32_t JMeshData::GetType() const
{
	return StringHash("JMeshData");
}

bool JMeshData::Serialize_Implement(std::ofstream& FileStream)
{
	if (!Utils::Serialization::SerializeMetaData(FileStream, this))
	{
		return false;
	}

	// Mesh Name
	Utils::Serialization::Serialize_Text(mName, FileStream);

	// Mesh Index
	Utils::Serialization::Serialize_Primitive(&mIndex, sizeof(mIndex), FileStream);

	// Mesh Type
	Utils::Serialization::Serialize_Primitive(&mClassType, sizeof(mClassType), FileStream);

	// Face Count
	Utils::Serialization::Serialize_Primitive(&mFaceCount, sizeof(mFaceCount), FileStream);

	// SubMesh
	size_t subMeshSize = mSubMesh.size();
	Utils::Serialization::Serialize_Primitive(&subMeshSize, sizeof(subMeshSize), FileStream);
	for (int32_t i = 0; i < mSubMesh.size(); ++i)
	{
		mSubMesh[i]->Serialize_Implement(FileStream);
	}

	// ChildMesh
	size_t childMeshSize = mChildMesh.size();
	Utils::Serialization::Serialize_Primitive(&childMeshSize, sizeof(childMeshSize), FileStream);
	for (int32_t i = 0; i < mChildMesh.size(); ++i)
	{
		mChildMesh[i]->Serialize_Implement(FileStream);
	}

	bool bVertexDataValid = mVertexData != nullptr;
	Utils::Serialization::Serialize_Primitive(&bVertexDataValid, sizeof(bVertexDataValid), FileStream);
	if (bVertexDataValid)
	{
		mVertexData->Serialize_Implement(FileStream);
	}

	// Material
	bool bMaterialValid = mMaterialInstance != nullptr;
	Utils::Serialization::Serialize_Primitive(&bMaterialValid, sizeof(bMaterialValid), FileStream);
	if (bMaterialValid)
	{
		JText path = mMaterialInstance->GetMaterialPath();
		Utils::Serialization::Serialize_Text(path, FileStream);
	}

	// Initial Transform
	Utils::Serialization::Serialize_Primitive(&mInitialModelTransform, sizeof(mInitialModelTransform), FileStream);

	return true;
}

bool JMeshData::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, GetType()))
	{
		return false;
	}

	// Mesh Name
	Utils::Serialization::DeSerialize_Text(mName, InFileStream);

	// Mesh Index
	Utils::Serialization::DeSerialize_Primitive(&mIndex, sizeof(mIndex), InFileStream);

	// Mesh Type
	Utils::Serialization::DeSerialize_Primitive(&mClassType, sizeof(mClassType), InFileStream);

	// Face Count
	Utils::Serialization::DeSerialize_Primitive(&mFaceCount, sizeof(mFaceCount), InFileStream);

	// SubMesh
	size_t subMeshSize;
	Utils::Serialization::DeSerialize_Primitive(&subMeshSize, sizeof(subMeshSize), InFileStream);
	mSubMesh.reserve(subMeshSize);
	for (int32_t i = 0; i < subMeshSize; ++i)
	{
		mSubMesh.push_back(std::make_shared<JMeshData>());
		mSubMesh[i]->DeSerialize_Implement(InFileStream);
	}

	// ChildMesh
	size_t childMeshSize;
	Utils::Serialization::DeSerialize_Primitive(&childMeshSize, sizeof(childMeshSize), InFileStream);
	mChildMesh.reserve(childMeshSize);
	for (int32_t i = 0; i < childMeshSize; ++i)
	{
		mChildMesh.push_back(MakePtr<JMeshData>());
		mChildMesh[i]->DeSerialize_Implement(InFileStream);
		mChildMesh[i]->mParentMesh = shared_from_this();
	}

	// VertexData
	bool bVertexDataValid;
	Utils::Serialization::DeSerialize_Primitive(&bVertexDataValid, sizeof(bVertexDataValid), InFileStream);
	mVertexData = bVertexDataValid ? std::make_shared<JVertexData<Vertex::FVertexInfo_Base>>() : nullptr;
	mVertexData->DeSerialize_Implement(InFileStream);

	// Material
	bool bMaterialValid;
	Utils::Serialization::DeSerialize_Primitive(&bMaterialValid, sizeof(bMaterialValid), InFileStream);
	if (bMaterialValid)
	{
		JText path;
		Utils::Serialization::DeSerialize_Text(path, InFileStream);
		mMaterialInstance = MMaterialInstanceManager::Get().CreateOrLoad(path);
	}


	// Initial Transform
	Utils::Serialization::DeSerialize_Primitive(&mInitialModelTransform, sizeof(mInitialModelTransform), InFileStream);

	return true;
}

void JMeshData::AddInfluenceBone(const JText& InBoneName, FMatrix InBindPose)
{
	mBindPoseMap.try_emplace(InBoneName, InBindPose);
}

void JMeshData::PassMaterial(ID3D11DeviceContext* InDeviceContext) const
{
	mMaterialInstance->BindMaterial(InDeviceContext);
}

void JSkinnedMeshData::Initialize()
{
	if (mVertexCount == 0 || mVertexStride == 0)
	{
		return;
	}

	const int32_t bufferSize = mVertexCount * mVertexStride;

	mBoneIndices.reset(new uint32_t[bufferSize]);
	mBoneWeights.reset(new float[bufferSize]);

	ZeroMemory(mBoneIndices.get(), sizeof(float) * bufferSize);
	ZeroMemory(mBoneWeights.get(), sizeof(float) * bufferSize);
}

void JSkinnedMeshData::Initialize(int32_t InVertexCount, int32_t InVertexStride)
{
	SetVertexCount(InVertexCount);
	SetVertexStride(InVertexStride);

	Initialize();
}

void JSkinnedMeshData::AddInfluenceBone(const JText& InBoneName)
{
	mInfluenceBones.push_back(InBoneName);
}

void JSkinnedMeshData::AddBindPose(const JText& InBoneName, const FMatrix& InBindPose)
{
	mBindPoseMap.try_emplace(InBoneName, InBindPose);
}

void JSkinnedMeshData::AddInverseBindPose(const JText& InBoneName, const FMatrix& InInverseBindPose)
{
	mInverseBindPoseMap.try_emplace(InBoneName, InInverseBindPose);
}

void JSkinnedMeshData::AddWeight(int32_t InIndex, uint32_t InBoneIndex, float InBoneWeight)
{}
