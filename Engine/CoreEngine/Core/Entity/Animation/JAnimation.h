#pragma once
#include "common_include.h"
#include "Core/Utils/Math/TMatrix.h"

namespace Utils::Fbx
{
	class FbxFile;
}

/**
 * 특정 키(시간) 에 추가 될 데이터 
 * @tparam T 키 프레임에 추가 될 데이터
 */
template <typename T>
struct JAnimationKey
{
	float Time;
	T     Value;
};

template <typename T>
using JAnimationKeyList = JArray<JAnimationKey<T>>;

/**
 * 변환 정보를 가지는 키 프레임 데이터
 */
struct FAnimKeyDataTransform
{
	JAnimationKeyList<FVector> PositionKeys;
	JAnimationKeyList<FVector> RotationKeys;
	JAnimationKeyList<FVector> ScaleKeys;
};

struct JAnimDataTransform
{
	FMatrix TransformMatrix;
	FVector Position;
	FVector Rotation;
	FVector Scale;
};

struct JKeyFrame
{
	JText                  Name;		// 이름
	JAnimDataTransform     Transform;	// 변환 정보
	JArray<Ptr<JKeyFrame>> ChildFrames;	// 자식 프레임
};

struct JAnimationTrack
{
	void AddKey(float InTime, const JAnimDataTransform& InTransformData);
	void AddKey(float InTime, const FMatrix& InTransformMatrix);
	void AddKey(float InTime, const FVector& InPosition, const FVector& InRotation, const FVector& InScale);

	void OptimizeKeys();
	void SortKeys();
	void EndianSwap();

	FAnimKeyDataTransform TransformKeys;
	Ptr<JKeyFrame>        SourceFrame;
};
