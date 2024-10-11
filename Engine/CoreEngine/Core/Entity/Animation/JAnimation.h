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
	JAnimationKeyList<FVector>  PositionKeys;
	JAnimationKeyList<FVector4> RotationKeys;
	JAnimationKeyList<FVector>  ScaleKeys;
};

struct JAnimDataTransform
{
	FMatrix  TransformMatrix;
	FVector  Position;
	FVector4 Rotation;
	FVector  Scale;
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
	void AddKey(float InTime, const FVector& InPosition, const FVector4& InRotation, const FVector& InScale);

	void OptimizeKeys();
	void OptimizeKey();
	void SortKeys();
	void EndianSwap();

	JText                 Name;
	FAnimKeyDataTransform TransformKeys;
	Ptr<JKeyFrame>        SourceFrame;
};

template <typename T>
bool LessEqual(const JAnimationKey<T> InKeyA, const JAnimationKey<T> InKeyB)
{
	return InKeyA.Time <= InKeyB.Time;
}

template <typename T>
FVector ComputeSlope(const JAnimationKey<T>& InKeyA, const JAnimationKey<T>& InKeyB)
{
	FVector     positionDelta = InKeyB.Value - InKeyA.Value;
	const float timeDelta     = InKeyB.Time - InKeyA.Time;

	positionDelta /= timeDelta;

	return positionDelta;
}

template <typename T>
bool NewSlopeEncountered(T InSlopeRef, T InNewSlope, const float InThreshold)
{
	T len = InSlopeRef.LengthSquared();

	return true;
}

class JAnimation
{
public:
	JAnimation();
	~JAnimation();

	void AddTrack(const Ptr<JAnimationTrack>& Track);
	void RemoveTrack(JAnimationTrack& Track);
	void RemoveTrack(uint32_t Index);
	void RemoveAllTracks();

	void SetStartTime(float Time);
	void SetEndTime(float Time);
	void SetSourceSamplingInterval(float Interval);

	float GetStartTime() const;
	float GetEndTime() const;
	float GetSourceSamplingInterval() const;

	JArray<JAnimationTrack>& GetTracks();

	void Evaluate(float Time, JArray<JAnimDataTransform>& OutTransforms);

protected:
	JText                        mName;
	float                        mStartTime;
	float                        mEndTime;
	float                        mSourceSamplingInterval;
	JArray<Ptr<JAnimationTrack>> mTracks;

	friend class Utils::Fbx::FbxFile;
};
