#pragma once
#include "common_pch.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Interface/IRenderable.h"
#include "Core/Utils/ObjectLoader/FbxFile.h"


/**
 * 화면에 뿌려지는 모든 데이터는 이 오브젝트를 컴포넌트로 가지거나 상속받는다
 */
class JDXObject : public IRenderable
{
public:
	explicit JDXObject();
	explicit JDXObject(Utils::Fbx::FbxFile* InFbxObj);
	~JDXObject() = default;

public:
	void Update(float DeltaTime);
	void Release();

#pragma region Render Interface
	void PreRender() override;
	void Render() override;
	void PostRender() override;
#pragma endregion

public:
	void UpdateVertexData(void*          InSrcData, const uint32_t InVertexNum, const uint32_t InVertexSize,
						  const uint32_t InVertexBegin);
	void UpdateIndexData(void* InSrcData, const uint32_t InIndexNum, const uint32_t InIndexBegin);

public:
	[[nodiscard]] FORCEINLINE ID3D11Buffer*            GetVertexBuffer() const { return mVertexBuffer.Get(); }
	[[nodiscard]] FORCEINLINE ID3D11Buffer*            GetIndexBuffer() const { return mIndexBuffer.Get(); }
	[[nodiscard]] FORCEINLINE ID3D11Buffer*            GetCBuffer() const { return mConstantBuffer_Space.Get(); }
	[[nodiscard]] FORCEINLINE D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType() const { return mPrimitiveType; }

	void SetShaderFile(JWTextView InFileName);
	void SetShaderFile(const JWText& InFileName);

	void SetWorldRotation(const float InRot) { mRotation.z = InRot; }

private:
	void CreateBuffers();
	void UpdateTransform();
	void UpdateSpaceBuffer();

private:
	JWText         mShaderFile;
	class JShader* mShader;

	// -------------------------------- Buffers --------------------------------------
	ComPtr<ID3D11Buffer> mVertexBuffer;			// Vertex 
	ComPtr<ID3D11Buffer> mIndexBuffer;			// Index
	ComPtr<ID3D11Buffer> mConstantBuffer_Space; // World, View, Projection
	ComPtr<ID3D11Buffer> mConstantBuffer_Light; // Light Direction
	ComPtr<ID3D11Buffer> mConstantBuffer_Time;  // Light Direction

	// ----------------------------- Model Primitive Data -----------------------------
	std::vector<Ptr<class JMesh>> mPrimitiveMeshData;

	uint32_t mVertexNum;
	uint32_t mIndexNum;
	uint32_t mVertexSize;
	uint32_t mIndexSize;
	uint32_t mVertexBegin;
	uint32_t mIndexBegin;

	FVector mTranslation = {0, 0.f, 0.f};
	FVector mRotation    = FVector::ZeroVector;
	FVector mScale       = 1.f;

	FMatrix mTranslationMat;
	FMatrix mRotationMat;
	FMatrix mScaleMat;
	FMatrix mWorldMat = FMatrix::Identity;

	D3D11_PRIMITIVE_TOPOLOGY mPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};
