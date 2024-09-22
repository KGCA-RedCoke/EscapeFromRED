#pragma once
#include "Core/Entity/Transform/JTransformComponent.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Interface/IRenderable.h"
#include "Core/Utils/ObjectLoader/FbxFile.h"


/**
 * 화면에 뿌려지는 모든 데이터는 이 오브젝트를 컴포넌트로 가지거나 상속받는다
 */
class J3DObject : public JTransformComponent, public IRenderable, public ISerializable
{
public:
	J3DObject() = default;
	explicit J3DObject(const JWText& ModelFileName);
	~J3DObject() override = default;

public:
	void Serialize(std::ofstream& FileStream) override;
	void DeSerialize(std::ifstream& InFileStream) override;

public:
	void Update(float DeltaTime);

#pragma region Render Interface
	void PreRender() override;
	void Render() override;
	void PostRender() override;
#pragma endregion

private:
	void CreateBuffers();
	void UpdateTransform();
	void UpdateBuffer();

private:
	// -------------------------------- Buffers --------------------------------------
	std::vector<Buffer::FBufferInstance> mInstanceBuffer;

	// ----------------------------- Model Primitive Data -----------------------------
	std::vector<Ptr<class JMeshData>> mPrimitiveMeshData;

	// ----------------------------- Model Data -----------------------------
	uint32_t mVertexSize;
	uint32_t mIndexSize;
	uint32_t mVertexBegin;
	uint32_t mIndexBegin;

	D3D11_PRIMITIVE_TOPOLOGY mPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};
