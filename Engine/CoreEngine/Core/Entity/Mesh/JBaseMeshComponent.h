// #pragma once
// #include "Core/Entity/Transform/JTransformComponent.h"
//
// class CFBXFile;
// struct JMesh;
//
// class JBaseMeshComponent : public JTransformComponent, public IRenderable
// {
// public:
// 	JBaseMeshComponent();
// 	JBaseMeshComponent(JTextView InName);
// 	~JBaseMeshComponent() override;
//
// public:
// 	void PreRender() override;
// 	void Render() override;
// 	void PostRender() override;
//
//
// protected:
// 	void SetBoneMatrices(JMesh* InMeshData, FbxFile* AnimMesh);
// protected:
//
// 	std::vector<Ptr<FbxData>>         mFbxData;
// 	std::vector<Ptr<struct JMesh>> mMeshData;
// };
