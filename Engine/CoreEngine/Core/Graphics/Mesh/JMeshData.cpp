#include "JMeshData.h"
#include "Core/Graphics/Material/JMaterial.h"
#include "Core/Graphics/Material/Instance/JMaterialInstance.h"
#include "Core/Graphics/Texture/JTexture.h"
#include "Core/Interface/MManagerInterface.h"

Ptr<IManagedInterface> JMeshData::Clone() const
{
	Ptr<JMeshData> clonedMesh = MakePtr<JMeshData>();

	// 이 값들은 그냥 l-value로 복사해도 무방하다.
	clonedMesh->mName                  = mName;
	clonedMesh->mIndex                 = mIndex;
	clonedMesh->mClassType             = mClassType;
	clonedMesh->mFaceCount             = mFaceCount;
	clonedMesh->mMaterialRefNum        = mMaterialRefNum;
	clonedMesh->mInitialModelTransform = mInitialModelTransform;

	// 부모 메시까지 같이 복사해야한다.
	if (mParentMesh.lock())
	{
		Ptr<JMeshData> parentMesh = mParentMesh.lock();
		clonedMesh->mParentMesh   = dynamic_pointer_cast<JMeshData>(parentMesh->Clone());
	}

	// 머티리얼은 복사하는게 아니라 인스턴스를 참조한다.
	if (mMaterialInstance)
	{
		clonedMesh->mMaterialInstance = mMaterialInstance;
	}
	// 버텍스 데이터도 마찬가지로 참조한다. (공유하고 행렬데이터는 따로 관리된다)
	if (mVertexData)
	{
		clonedMesh->mVertexData = mVertexData;
	}

	clonedMesh->mSubMesh.reserve(mSubMesh.size());
	clonedMesh->mChildMesh.reserve(mChildMesh.size());

	// 서브 메시와 자식 메시는 재귀적으로 확인
	for (int32_t i = 0; i < mSubMesh.size(); ++i)
	{
		clonedMesh->mSubMesh.push_back(std::dynamic_pointer_cast<JMeshData>(mSubMesh[i]->Clone()));
	}

	for (int32_t i = 0; i < mChildMesh.size(); ++i)
	{
		clonedMesh->mChildMesh.push_back(std::dynamic_pointer_cast<JMeshData>(mChildMesh[i]->Clone()));
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

void JMeshData::UpdateWorldMatrix(ID3D11DeviceContext* InDeviceContext, const FMatrix& InWorldMatrix) const
{
	mMaterialInstance->UpdateWorldMatrix(InDeviceContext, InWorldMatrix);
}

void JMeshData::PassMaterial(ID3D11DeviceContext* InDeviceContext) const
{
	mMaterialInstance->BindMaterial(InDeviceContext);
}

bool JSkeletalMesh::Serialize_Implement(std::ofstream& FileStream)
{
	JMeshData::Serialize_Implement(FileStream);

	// Skeletal Mesh Data
	int32_t jointSize = mSkeletonData.Joints.size();
	Utils::Serialization::Serialize_Primitive(&jointSize, sizeof(jointSize), FileStream);
	for (auto& joint : mSkeletonData.Joints)
	{
		Utils::Serialization::Serialize_Text(joint.Name, FileStream);
		Utils::Serialization::Serialize_Primitive(&joint.ParentIndex, sizeof(joint.ParentIndex), FileStream);
	}

	// Skin Data
	bool bSkinDataValid = mSkinData != nullptr;
	Utils::Serialization::Serialize_Primitive(&bSkinDataValid, sizeof(bSkinDataValid), FileStream);
	if (bSkinDataValid)
	{
		mSkinData->Serialize_Implement(FileStream);
	}

	return true;
}

bool JSkeletalMesh::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JMeshData::DeSerialize_Implement(InFileStream);

	// Skeletal Mesh Data
	int32_t jointSize;
	Utils::Serialization::DeSerialize_Primitive(&jointSize, sizeof(jointSize), InFileStream);
	mSkeletonData.Joints.reserve(jointSize);
	for (int32_t i = 0; i < jointSize; ++i)
	{
		JText jointName;
		Utils::Serialization::DeSerialize_Text(jointName, InFileStream);
		int32_t parentIndex;
		Utils::Serialization::DeSerialize_Primitive(&parentIndex, sizeof(parentIndex), InFileStream);
		mSkeletonData.Joints.push_back({jointName, parentIndex});
	}

	// Skin Data
	bool bSkinDataValid;
	Utils::Serialization::DeSerialize_Primitive(&bSkinDataValid, sizeof(bSkinDataValid), InFileStream);
	if (bSkinDataValid)
	{
		mSkinData = MakePtr<JSkinData>();
		mSkinData->DeSerialize_Implement(InFileStream);
	}

	return true;
}

void JSkeletalMesh::AddInfluenceBone(const JText& InBoneName, FMatrix InBindPose)
{
	if (!mSkinData)
	{
		LOG_CORE_ERROR("Skin Data is not initialized.");
		return;
	}

	mSkinData->AddBindPose(InBoneName, InBindPose);
}

void JSkeletalMesh::SetSkeletalData(const FSkeletonData& InSkeletonData)
{
	mSkeletonData = InSkeletonData;
}

void JSkeletalMesh::SetSkinData(const Ptr<JSkinData>& InSkinData)
{
	mSkinData = InSkinData;
}

JSkinData::~JSkinData()
{
	mBoneIndices = nullptr;
	mBoneWeights = nullptr;
}

uint32_t JSkinData::GetType() const
{
	return StringHash("JSkinData");
}

bool JSkinData::Serialize_Implement(std::ofstream& FileStream)
{
	if (!Utils::Serialization::SerializeMetaData(FileStream, this))
	{
		return false;
	}

	// Max Bone Influences
	Utils::Serialization::Serialize_Primitive(&MAX_BONE_INFLUENCES, sizeof(MAX_BONE_INFLUENCES), FileStream);

	int32_t influenceBoneSize = mInfluenceBones.size();
	Utils::Serialization::Serialize_Primitive(&influenceBoneSize, sizeof(influenceBoneSize), FileStream);
	for (auto& bone : mInfluenceBones)
	{
		Utils::Serialization::Serialize_Text(bone, FileStream);
	}

	int32_t bindPoseSize = mBindPoseWorldMap.size();
	Utils::Serialization::Serialize_Primitive(&bindPoseSize, sizeof(bindPoseSize), FileStream);
	for (auto& bindPose : mBindPoseWorldMap)
	{
		JText boneName = bindPose.first;
		Utils::Serialization::Serialize_Text(boneName, FileStream);
		Utils::Serialization::Serialize_Primitive(&bindPose.second, sizeof(bindPose.second), FileStream);
	}

	int32_t inverseBindPoseSize = mInverseBindPoseMap.size();
	Utils::Serialization::Serialize_Primitive(&inverseBindPoseSize, sizeof(inverseBindPoseSize), FileStream);
	for (auto& inverseBindPose : mInverseBindPoseMap)
	{
		JText boneName = inverseBindPose.first;
		Utils::Serialization::Serialize_Text(boneName, FileStream);
		Utils::Serialization::Serialize_Primitive(&inverseBindPose.second, sizeof(inverseBindPose.second), FileStream);
	}

	return true;
}

bool JSkinData::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, GetType()))
	{
		return false;
	}

	// Max Bone Influences
	Utils::Serialization::DeSerialize_Primitive(&MAX_BONE_INFLUENCES, sizeof(MAX_BONE_INFLUENCES), InFileStream);

	// Influence Bones
	int32_t influenceBoneSize;
	Utils::Serialization::DeSerialize_Primitive(&influenceBoneSize, sizeof(influenceBoneSize), InFileStream);
	mInfluenceBones.reserve(influenceBoneSize);
	for (int32_t i = 0; i < influenceBoneSize; ++i)
	{
		JText boneName;
		Utils::Serialization::DeSerialize_Text(boneName, InFileStream);
		mInfluenceBones.push_back(boneName);
	}

	// Bind Pose
	int32_t bindPoseSize;
	Utils::Serialization::DeSerialize_Primitive(&bindPoseSize, sizeof(bindPoseSize), InFileStream);
	for (int32_t i = 0; i < bindPoseSize; ++i)
	{
		JText boneName;
		Utils::Serialization::DeSerialize_Text(boneName, InFileStream);
		FMatrix bindPose;
		Utils::Serialization::DeSerialize_Primitive(&bindPose, sizeof(bindPose), InFileStream);
		mBindPoseWorldMap.try_emplace(boneName, bindPose);
	}

	// Inverse Bind Pose
	int32_t inverseBindPoseSize;
	Utils::Serialization::DeSerialize_Primitive(&inverseBindPoseSize, sizeof(inverseBindPoseSize), InFileStream);
	for (int32_t i = 0; i < inverseBindPoseSize; ++i)
	{
		JText boneName;
		Utils::Serialization::DeSerialize_Text(boneName, InFileStream);
		FMatrix inverseBindPose;
		Utils::Serialization::DeSerialize_Primitive(&inverseBindPose, sizeof(inverseBindPose), InFileStream);
		mInverseBindPoseMap.try_emplace(boneName, inverseBindPose);
	}

	return true;
}

void JSkinData::Initialize()
{
	if (mVertexCount == 0 || mVertexStride == 0)
	{
		return;
	}

	/// 버퍼의 사이즈는 몇이 될까?
	/// 영향을 받는 정점 갯수 * (영향을 받는 본의 최대 갯수(8)) 
	const int32_t bufferSize = mVertexCount * mVertexStride;

	mBoneIndices = MakeUPtr<int32_t[]>(bufferSize);
	mBoneWeights = MakeUPtr<float[]>(bufferSize);

	ZeroMemory(mBoneIndices.get(), sizeof(uint32_t) * bufferSize);
	ZeroMemory(mBoneWeights.get(), sizeof(float) * bufferSize);
}

int32_t JSkinData::MAX_BONE_INFLUENCES = 0;

void JSkinData::Initialize(int32_t InVertexCount, int32_t InVertexStride)
{
	SetVertexCount(InVertexCount);
	SetVertexStride(InVertexStride);

	Initialize();
}

void JSkinData::AddInfluenceBone(const JText& InBoneName)
{
	mInfluenceBones.push_back(InBoneName);
}

void JSkinData::AddBindPose(const JText& InBoneName, const FMatrix& InBindPose)
{
	mBindPoseWorldMap.try_emplace(InBoneName, InBindPose);
}

void JSkinData::AddInverseBindPose(const JText& InBoneName, const FMatrix& InInverseBindPose)
{
	mInverseBindPoseMap.try_emplace(InBoneName, InInverseBindPose);
}


void JSkinData::AddWeight(int32_t InIndex, int32_t InBoneIndex, float InBoneWeight)
{
	assert(InBoneIndex < 256);

	int32_t* indices = GetBoneIndex(InIndex);
	float*   weights = GetBoneWeight(InIndex);

	for (int32_t i = 0; i < mVertexStride; ++i)
	{
		if (InBoneWeight > weights[i])
		{
			for (int32_t j = mVertexStride - 1; j > i; --j)
			{
				indices[j] = indices[j - 1];
				weights[j] = weights[j - 1];
			}
			indices[i] = InBoneIndex;
			weights[i] = InBoneWeight;
			break;
		}

		if (MAX_BONE_INFLUENCES < i)
		{
			MAX_BONE_INFLUENCES = i;
		}
	}
}

int32_t* JSkinData::GetBoneIndex(int32_t InIndex) const
{
	assert(InIndex < mVertexCount);

	return mBoneIndices.get() + InIndex * mVertexStride;
}

float* JSkinData::GetBoneWeight(int32_t InIndex) const
{
	assert(InIndex < mVertexCount);

	return mBoneWeights.get() + InIndex * mVertexStride;
}
