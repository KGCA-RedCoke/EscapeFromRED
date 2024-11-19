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

	FVector mNearCenter = FVector::ZeroVector;	// Near Plane의 중심좌표
	FVector mFarCenter  = FVector::ZeroVector;	// Far Plane의 중심좌표

	FVector                          mFrustum[8];
	JArray<Vertex::FVertexInfo_Base> mVertexData;

};
