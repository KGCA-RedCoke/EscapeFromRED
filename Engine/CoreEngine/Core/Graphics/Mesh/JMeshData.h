#pragma once
#include "common_include.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Utils/FileIO/JSerialization.h"

class JMaterial;
namespace Utils::Fbx
{
	class FbxFile;
}

enum class EMeshType : uint8_t
{
	Static = 0, // 정적 메시
	Skeletal,   // 스켈레탈 메시
	DUMMY, 		// 더미 메시
	BIPED		// 바이패드 메시 (뭔지는 모르겠음 maya에서 관리하는것같음)
};

/**
 * Mesh 
 */
class JMeshData : public ISerializable, public std::enable_shared_from_this<JMeshData>// 직렬화 해서 저장
{
public:
	JMeshData() = default;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	[[nodiscard]] bool ApplyMaterial() const;

public:
	[[nodiscard]] JText GetName() const { return mName; }
	[[nodiscard]] int32_t GetIndex() const { return mIndex; }
	[[nodiscard]] EMeshType GetClassType() const { return mClassType; }
	[[nodiscard]] int32_t GetMaterialRefNum() const { return mMaterialRefNum; }
	[[nodiscard]] int32_t GetFaceNum() const { return mFaceCount; }
	[[nodiscard]] Ptr<JMeshData> GetParentMesh() const { return mParentMesh.lock(); }
	[[nodiscard]] const std::vector<Ptr<JMeshData>>& GetSubMesh() const { return mSubMesh; }
	[[nodiscard]] const std::vector<Ptr<JMeshData>>& GetChildMesh() const { return mChildMesh; }
	[[nodiscard]] const Ptr<JVertexData<Vertex::FVertexInfo_Base>>& GetVertexData() const { return mVertexData; }
	[[nodiscard]] Ptr<JMaterial> GetMaterial() const { return mMaterial; }
	[[nodiscard]] FMatrix GetInitialModelTransform() const { return mInitialModelTransform; }

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

	// -------------------------- Material --------------------------
	Ptr<JMaterial> mMaterial;
	int32_t        mMaterialRefNum;

	// -------------------------- Initial Model Transform --------------------------
	FMatrix mInitialModelTransform;

private:
	friend class Utils::Fbx::FbxFile;
	friend class GUI_Editor_Material;
};

/**
 * Skin Mesh
 */
class JSkinnedMesh
{};
