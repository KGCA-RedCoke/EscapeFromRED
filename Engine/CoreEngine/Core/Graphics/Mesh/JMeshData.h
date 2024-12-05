#pragma once
#include "common_include.h"
#include "Core/Entity/Component/Scene/Shape/JShape.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Manager/IManagedInterface.h"
#include "Core/Utils/FileIO/JSerialization.h"

class JMaterialInstance;
class JMaterial;
namespace Utils::Fbx
{
	class FbxFile;
}

enum class EMeshType : uint8_t
{
	Static       = 1 << 10, // 정적 메시
	Skeletal     = 1 << 11,   // 스켈레탈 메시
	AnimatedMesh = 1 << 12, // 애니메이션 메시
};

/**
 * Mesh 
 */
class JMeshData : public ISerializable,
				  public IManagedInterface,
				  public std::enable_shared_from_this<JMeshData>// 직렬화 해서 저장
{
public:
	JMeshData() = default;
	JMeshData(const JMeshData& Other);

public:
	UPtr<IManagedInterface> Clone() const override;

public:
	uint32_t GetHash() const override;
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	virtual void AddInfluenceBone(const JText& InBoneName, FMatrix InBindPose) {};
	virtual void SetSkeletalData(const class FSkeletonData& InSkeletonData) {}
	virtual void SetSkinData(const Ptr<class JSkinData>& InSkinData) {}

public:
	[[nodiscard]] JText GetName() const { return mName; }
	[[nodiscard]] int32_t GetIndex() const { return mIndex; }
	[[nodiscard]] EMeshType GetClassType() const { return mClassType; }
	[[nodiscard]] int32_t GetFaceNum() const { return mFaceCount; }
	[[nodiscard]] Ptr<JMeshData> GetParentMesh() const { return mParentMesh.lock(); }
	[[nodiscard]] const JArray<Ptr<JMeshData>>& GetSubMesh() const { return mSubMesh; }
	[[nodiscard]] const JArray<Ptr<JMeshData>>& GetChildMesh() const { return mChildMesh; }
	[[nodiscard]] const Ptr<JVertexData<Vertex::FVertexInfo_Base>>& GetVertexData() const { return mVertexData; }
	[[nodiscard]] const FBoxShape& GetBoundingBox() const { return mBoundingBox; }
	[[nodiscard]] FMatrix GetInitialModelTransform() const { return mInitialModelTransform; }
	[[nodiscard]] FORCEINLINE int32_t GetSubMaterialNum() const { return mMaterialRefNum; }

	[[nodiscard]] FORCEINLINE bool IsSkeletalMesh() const { return mClassType == EMeshType::Skeletal; }
	[[nodiscard]] FORCEINLINE bool IsStaticMesh() const { return mClassType == EMeshType::Static; }

protected:
	// -------------------------- Mesh Info --------------------------
	JText     mName;
	int32_t   mIndex;
	EMeshType mClassType;

	// -------------------------- Face Count --------------------------
	int32_t mFaceCount;

	// -------------------------- Mesh Hierarchy --------------------------
	WPtr<JMeshData>        mParentMesh;
	JArray<Ptr<JMeshData>> mSubMesh;
	JArray<Ptr<JMeshData>> mChildMesh;

	// -------------------------- Vertex Data --------------------------
	Ptr<JVertexData<Vertex::FVertexInfo_Base>> mVertexData;

	// -------------------------- Bounding Box --------------------------
	FBoxShape mBoundingBox;

	// DEPRECATED: 2024-11-17 이후로 JEngine에서 머티리얼과 메시는 완전히 분리 (하지만 머티리얼 갯수는 여전히 필요)
	// -------------------------- Material Reference --------------------------
	// JMaterialInstance* mMaterialInstances;
	int32_t mMaterialRefNum;

	// -------------------------- Initial Model Transform --------------------------
	FMatrix mInitialModelTransform;

private:
	friend class Utils::Fbx::FbxFile;
	friend class GUI_Editor_Material;
};

/**
	 * 관절 데이터
	 */
struct FJointData
{
	JText   Name;					// 조인트 이름
	int32_t ParentIndex = -1;		// 부모 본
};

struct FSkeletonData
{
	JArray<FJointData> Joints;
};

class JSkeletalMesh : public JMeshData
{
public:
	bool Serialize_Implement(std::ofstream& FileStream) override;
	bool DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void AddInfluenceBone(const JText& InBoneName, FMatrix InBindPose) override;
	void SetSkeletalData(const class FSkeletonData& InSkeletonData) override;
	void SetSkinData(const Ptr<class JSkinData>& InSkinData) override;

	[[nodiscard]] FORCEINLINE const FSkeletonData&  GetSkeletonData() const { return mSkeletonData; }
	[[nodiscard]] FORCEINLINE const Ptr<JSkinData>& GetSkinData() const { return mSkinData; }

protected:
	// -------------------------- Skin Mesh Data --------------------------
	FSkeletonData        mSkeletonData;	// 본 계층 구조
	Ptr<class JSkinData> mSkinData;		// 스킨 데이터(본과 메시를 연결)

	friend class Utils::Fbx::FbxFile;
};

/**
 * Skin Mesh
 */
class JSkinData : public JAsset
{
public:
	JSkinData() = default;
	~JSkinData() override;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Initialize();
	void Initialize(int32_t InVertexCount, int32_t InVertexStride);

	void AddInfluenceBone(const JText& InBoneName);
	void AddBindPose(const JText& InBoneName, const FMatrix& InBindPose);
	void AddInverseBindPose(const JText& InBoneName, const FMatrix& InInverseBindPose);
	void AddWeight(int32_t InIndex, int32_t InBoneIndex, float InBoneWeight);

	FORCEINLINE void SetVertexCount(int32_t InVertexCount) { mVertexCount = InVertexCount; }
	FORCEINLINE void SetVertexStride(int32_t InVertexStride) { mVertexStride = InVertexStride; }

	[[nodiscard]] FORCEINLINE const JHash<JText, FMatrix>& GetBindPoseWorldMap() const { return mBindPoseWorldMap; }
	[[nodiscard]] FORCEINLINE const JHash<JText, FMatrix>& GetInverseBindPoseMap() const { return mInverseBindPoseMap; }

	[[nodiscard]] FORCEINLINE JText   GetInfluenceBoneName(int32_t InIndex) const { return mInfluenceBones.at(InIndex); }
	[[nodiscard]] FORCEINLINE int32_t GetInfluenceBoneCount() const { return mInfluenceBones.size(); }

	[[nodiscard]] FORCEINLINE FMatrix GetInfluenceWorldBindPose(const JText& InBoneName) const
	{
		return mBindPoseWorldMap.at(InBoneName);
	}

	[[nodiscard]] FORCEINLINE FMatrix GetInfluenceBoneInverseBindPose(const JText& InBoneName) const
	{
		return mInverseBindPoseMap.at(InBoneName);
	}

	[[nodiscard]] FORCEINLINE int32_t GetVertexCount() const { return mVertexCount; }
	[[nodiscard]] FORCEINLINE int32_t GetVertexStride() const { return mVertexStride; }
	[[nodiscard]] FORCEINLINE int32_t GetBoneCount() const { return mInfluenceBones.size(); }

	int32_t* GetBoneIndex(int32_t InIndex) const;
	float*   GetBoneWeight(int32_t InIndex) const;

public:
	static int32_t MAX_BONE_INFLUENCES;

private:
	JArray<JText> mInfluenceBones;

	JHash<JText, FMatrix> mBindPoseWorldMap;
	JHash<JText, FMatrix> mInverseBindPoseMap;

	int32_t mVertexCount  = 0;
	int32_t mVertexStride = 0;

	UPtr<int32_t[]> mBoneIndices;
	UPtr<float[]>   mBoneWeights;

};
