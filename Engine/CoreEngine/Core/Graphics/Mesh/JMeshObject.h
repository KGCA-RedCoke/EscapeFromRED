#pragma once
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Graphics/Mesh/JMeshData.h"
#include "Core/Interface/IRenderable.h"
#include "Core/Utils/FileIO/JSerialization.h"
#include "Core/Utils/ObjectLoader/FbxFile.h"


class JCamera;
/**
 * 화면에 뿌려지는 모든 데이터는 이 오브젝트를 컴포넌트로 가지거나 상속받는다
 */
class JMeshObject : public ISerializable, public IRenderable
{
public:
	JMeshObject(const JText& InName, const std::vector<Ptr<JMeshData>>& InData = {});
	JMeshObject(const JWText& InName, const std::vector<Ptr<JMeshData>>& InData = {});
	~JMeshObject() override = default;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Update(float DeltaTime);

#pragma region Render Interface
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void Draw() override;
#pragma endregion

private:
	void CreateBuffers();

public:
	void UpdateBuffer(const FMatrix&  InWorldMatrix = FMatrix::Identity, const Ptr<JCamera>& InCameraObj = nullptr,
					  const FVector4& InLightLoc    = {500, 1200, 1000, 1},
					  const FVector4& InLightColor  = {1.f, 0.976f, 0.992f, 1},
					  const FVector4& InWorldTime   = FVector4::ZeroVector);

private:
	// -------------------------------- Buffers --------------------------------------
	JArray<Buffer::FBufferInstance> mInstanceBuffer;

	// ----------------------------- Model Primitive Data -----------------------------
	JArray<Ptr<JMeshData>> mPrimitiveMeshData;

	// ----------------------------- Model Data -----------------------------
	uint32_t mVertexSize;
	uint32_t mIndexSize;

	D3D11_PRIMITIVE_TOPOLOGY mPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

private:
	friend class Utils::Fbx::FbxFile;
	friend class GUI_Editor_Material;
};
