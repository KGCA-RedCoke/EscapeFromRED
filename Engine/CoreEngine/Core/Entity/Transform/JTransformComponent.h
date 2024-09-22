#pragma once
#include "Core/Utils/Math/TMatrix.h"


class JTransformComponent
{
public:
	/** 축을 Render */
	void RenderAxis();

public:
	FORCEINLINE [[nodiscard]] FVector GetTranslation() const { return mTranslation; }
	FORCEINLINE [[nodiscard]] FVector GetRotation() const { return mRotation; }
	FORCEINLINE [[nodiscard]] FVector GetScale() const { return mScale; }

	void SetTranslation(const FVector& InTranslation) { mTranslation = InTranslation; }
	void SetRotation(const FVector& InRotation) { mRotation = InRotation; }
	void SetScale(const FVector& InScale) { mScale = InScale; }
	
protected:
	// ----------------------------- Model Transform Data -----------------------------
	FVector mTranslation = FVector::ZeroVector;
	FVector mRotation    = FVector::ZeroVector;
	FVector mScale       = FVector::OneVector;

	FMatrix mTranslationMat = FMatrix::Identity;
	FMatrix mRotationMat    = FMatrix::Identity;
	FMatrix mScaleMat       = FMatrix::Identity;
	FMatrix mWorldMat       = FMatrix::Identity;

private:
	
};
