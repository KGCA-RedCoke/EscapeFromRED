#pragma once
#include "common_include.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Utils/FileIO/JSerialization.h"

namespace Utils::Fbx
{
	class FbxFile;
}

enum class EMeshType : uint8_t
{
	GEOM = 0,
	BONE,
	DUMMY,
	BIPED
};


/**
 * Mesh 
 */
class JMesh : public ISerializable // 직렬화 해서 저장
{
public:
	JMesh() = default;

public:
	void Serialize(std::ofstream& FileStream) override;
	void DeSerialize(std::ifstream& InFileStream) override;

public:
	[[nodiscard]] bool ApplyMaterial() const;

public:
	[[nodiscard]] JText                                GetName() const { return mName; }
	[[nodiscard]] int32_t                              GetIndex() const { return mIndex; }
	[[nodiscard]] EMeshType                            GetClassType() const { return mClassType; }
	[[nodiscard]] int32_t                              GetMaterialRefNum() const { return mMaterialRefNum; }
	[[nodiscard]] int32_t                              GetFaceNum() const { return mFaceNum; }
	[[nodiscard]] Ptr<JMesh>                           GetParentMesh() const { return mParentMesh; }
	[[nodiscard]] std::vector<Ptr<JMesh>>              GetSubMesh() const { return mSubMesh; }
	[[nodiscard]] std::vector<JMesh*>                  GetChildMesh() const { return mChildMesh; }
	[[nodiscard]] Ptr<JData<Vertex::FVertexInfo_Base>> GetVertexData() const { return mVertexData; }
	[[nodiscard]] Ptr<class JMaterial>                 GetMaterial() const { return mMaterial; }

private:
	JText     mName;
	int32_t   mIndex;
	EMeshType mClassType;

	int32_t mMaterialRefNum;
	int32_t mFaceNum;

	Ptr<JMesh>              mParentMesh;
	std::vector<Ptr<JMesh>> mSubMesh;
	std::vector<Ptr<JMesh>>     mChildMesh;

	Ptr<JData<Vertex::FVertexInfo_Base>> mVertexData;
	Ptr<class JMaterial>                 mMaterial;

private:
	friend class Utils::Fbx::FbxFile;
};
