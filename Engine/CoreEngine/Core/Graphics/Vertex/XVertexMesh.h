// #pragma once
// #include "Core/Graphics/ShaderStructs.h"
// #include "Core/Interface/ICoreInterface.h"
// #include "Core/Interface/IRenderable.h"
// #include "Core/Utils/Math/Color.h"
//
//
// class XVertexMesh : public ICoreInterface, public IRenderable
// {
// public:
// // 	XVertexMesh();
// // 	XVertexMesh(JWTextView InFbxFile);
// // 	~XVertexMesh() override;
// //
// // public:
// // #pragma region Core Interface
// // 	void Initialize() override;
// // 	void Update(float DeltaTime) override;
// // 	void Release() override;
// // #pragma endregion
// //
// // #pragma region Render Interface
// // 	void       PreRender() override;
// // 	void       Render() override;
// // 	void       PostRender() override;
// // 	ELayerType GetLayerType() override;
// // #pragma endregion
// //
// // 	void SetMesh(JWTextView InFbxFile);
// //
// // protected:
// // 	virtual void CreateVertexBuffer();
// // 	virtual void CreateIndexBuffer();
// // 	virtual void CreateVertexArray();
// // 	virtual void LoadShader(JWTextView InShaderFileName);
// //
// // protected:
// // 	JWText   mShaderFileName;
// // 	XShader* mShader;
// //
// // 	std::vector<FMeshData> mMeshDatas;
// //
// // 	D3D_PRIMITIVE_TOPOLOGY mPrimitiveType; /** 그려질 타입 */
// // 	FLinearColor           mDrawColor;     /** 텍스처를 안쓸때 지정 컬러 */
// };
