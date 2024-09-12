// #pragma once
// #include "Core/Entity/JObject.h"
// #include "Core/Graphics/ShaderStructs.h"
// #include "Core/Interface/IRenderable.h"
// #include "Core/Utils/Math/TMatrix.h"
// #include "Core/Utils/ObjectLoader/FbxObject.h"
//
// class JDXObject;
// struct JMesh;
// class CFBXFile;
// /**
//  * 위치를 가지는 컴포넌트
//  * local axis를 시각화 하기 위해 mesh를 추가
//  */
// class JTransformComponent : public JObject, public IRenderable
// {
// public:
// 	JTransformComponent();
//
// public:
// 	void         PreRender() override;
// 	void         Render() override;
// 	void         PostRender() override;
// 	
// 	virtual void SetMesh(FbxFile* InFbxObj);
//
// public:
// 	virtual void UpdateConstantBuffer();
//
// private:
//
// protected:
// 	JTransformComponent* mParentTransformComp;
// 	JDXObject*           mDXObject;
//
// 	CBuffer::Space mConstantBufferData;
//
// 	FMatrix mXFromWorldMat;
// 	FMatrix mWorldMat;
// 	FMatrix mWorldMat_Inverse;
// 	FMatrix mWorldTransformMat;
// 	FMatrix mWorldRotationMat;
// 	FMatrix mWorldScaleMat;
//
// 	std::vector<Ptr<VertexInfoData>>  mDataList;
// 	std::vector<Ptr<JMesh>> mMeshList;
//
// 	ComPtr<ID3D11Buffer>             mBoneBuffer;
// 	ComPtr<ID3D11ShaderResourceView> mBoneBufferSRV;
// };
