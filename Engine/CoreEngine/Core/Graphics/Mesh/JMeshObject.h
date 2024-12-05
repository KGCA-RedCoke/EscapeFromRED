#pragma once
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Interface/IRenderable.h"
#include "Core/Utils/ObjectLoader/FbxFile.h"

constexpr const char* Path_Mesh_Sphere   = "Game/Mesh/Sphere.jasset";
constexpr const char* Path_Mesh_Cube     = "Game/Mesh/Cube.jasset";
constexpr const char* Path_Mesh_Cylinder = "Game/Mesh/Cylinder.jasset";
constexpr const char* Path_Mesh_Plane    = "Game/Mesh/Plane.jasset";
constexpr const char* Path_Mesh_Circle   = "Game/Mesh/Circle.jasset";

/**
 * 메시데이터를 가지고있고 버퍼를 생성한다.
 */
class JMeshObject : public JAsset,
					public IManagedInterface,
					public IRenderable
{
public:
	JMeshObject() = default;
	JMeshObject(const JText& InAssetPath, const JArray<Ptr<JMeshData>>& InData = {});
	JMeshObject(const JWText& InAssetPath, const JArray<Ptr<JMeshData>>& InData = {});
	JMeshObject(const JMeshObject& Other);
	~JMeshObject() override = default;

public:
	UPtr<IManagedInterface> Clone() const override;

public:
	uint32_t GetHash() const override;
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

	JText GetName() const { return ParseFile(mName); }

public:
	virtual void Tick(float DeltaTime) {};

public:
#pragma region Render Interface
	void PreRender() override {}
	void AddInstance(float InCameraDistance) override;
	void PostRender() override {}
	void Draw() override;
	void DrawID(uint32_t ID) override;
#pragma endregion

public:
	void UpdateInstance_Transform(const FMatrix& InWorldMatrix = FMatrix::Identity);

	void    SetMaterialInstance(JMaterialInstance* InMaterialInstance, uint32_t InIndex = 0);
	int32_t GetMaterialCount() const;

	JMaterialInstance* GetMaterialInstance(uint32_t InIndex = 0) const;
	JMaterialInstance* GetMaterialInstance(const JText& InName) const;
	FBoxShape          GetBoundingBox() const { return mBoundingBox; }

protected:
	// ----------------------------- Model Data -----------------------------
	JText mName;

	// -------------------------------- Buffers --------------------------------------
	JArray<Buffer::FBufferGeometry> mGeometryBuffer;

	// ----------------------------- Model Primitive Data -----------------------------
	JArray<Ptr<JMeshData>> mPrimitiveMeshData;
	uint32_t               mVertexSize = sizeof(Vertex::FVertexInfo_Base);
	FBoxShape              mBoundingBox;

	// ----------------------------- Material Reference -----------------------------
	JArray<JMaterialInstance*> mMaterialInstances;
	JArray<FInstanceData_Mesh> mInstanceData;
	JHash<uint32_t, size_t>    mDelegateIDs;

private:
	friend class Utils::Fbx::FbxFile;
	friend class GUI_Editor_Mesh;
	friend class GUI_Editor_Material;
	friend class MMeshManager;
};
