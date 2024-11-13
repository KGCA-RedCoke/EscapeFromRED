#pragma once
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Interface/IRenderable.h"
#include "Core/Utils/ObjectLoader/FbxFile.h"

class JCamera;

constexpr const char* Path_Mesh_Sphere   = "Game/Mesh/Sphere.jasset";
constexpr const char* Path_Mesh_Cube     = "Game/Mesh/Cube.jasset";
constexpr const char* Path_Mesh_Cylinder = "Game/Mesh/Cylinder.jasset";
constexpr const char* Path_Mesh_Plane    = "Game/Mesh/Plane.jasset";
constexpr const char* Path_Mesh_Circle   = "Game/Mesh/Circle.jasset";

/**
 * 메시데이터를 가지고있고 버퍼를 생성한다.
 */
class JMeshObject : public IManagedInterface,
					public ISerializable,
					public IRenderable
{
public:
	JMeshObject() = default;
	JMeshObject(const JText& InName, const std::vector<Ptr<JMeshData>>& InData = {});
	JMeshObject(const JWText& InName, const std::vector<Ptr<JMeshData>>& InData = {});
	JMeshObject(const JMeshObject& Other);
	~JMeshObject() override = default;

public:
	Ptr<IManagedInterface> Clone() const override;

public:
	uint32_t GetHash() const override;
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	FMatrix GetWorldMatrix() const { return mWorldMatrix; }
	JText   GetName() const { return ParseFile(mName); }

public:
	virtual void Tick(float DeltaTime) {};

public:
#pragma region Render Interface
	void Draw(ID3D11DeviceContext* InDeviceContext) override;
	void DrawID(ID3D11DeviceContext* InDeviceContext, uint32_t ID) override;
#pragma endregion

public:
	void UpdateBuffer(const FMatrix& InWorldMatrix = FMatrix::Identity);

protected:
	// ----------------------------- Model Data -----------------------------
	JText   mName;
	FMatrix mWorldMatrix = FMatrix::Identity;

	// -------------------------------- Buffers --------------------------------------
	CBuffer::MeshConstantBuffer mMeshConstantBuffer;

	// ----------------------------- Model Primitive Data -----------------------------
	JArray<Ptr<JMeshData>> mPrimitiveMeshData;
	uint32_t               mVertexSize;

private:
	friend class Utils::Fbx::FbxFile;
	friend class GUI_Editor_Mesh;
	friend class GUI_Editor_Material;
	friend class MMeshManager;
};
