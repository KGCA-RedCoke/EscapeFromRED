#pragma once
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Interface/ICoreInterface.h"
#include "Core/Utils/Math/TMatrix.h"

class JFrustum : public ICoreInterface
{
public:


public:
	void Initialize() override;
	void Update(float DeltaTime) override;
	void Release() override;

private:
	FMatrix mWorldMatrix;
	FMatrix mViewMatrix;
	FMatrix mProjMatrix;

	FVector                          mFrustum[8];
	JArray<Vertex::FVertexInfo_Base> mVertexData;

};
