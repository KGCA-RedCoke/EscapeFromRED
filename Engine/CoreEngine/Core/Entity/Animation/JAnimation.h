#pragma once
#include "common_include.h"
#include "Core/Utils/Math/TMatrix.h"

namespace Utils::Fbx
{
	class FbxFile;
}

template <typename T>
struct JAnimationKey
{
	float Time;
	T     Value;
};

template <typename T>
using JAnimationKeyList = JArray<JAnimationKey<T>>;

struct JAnimDataTransform
{
	FMatrix TransformMatrix;
	FVector Position;
	FVector Rotation;
	FVector Scale;
};


struct JAnimationTrack
{
	JAnimationKeyList<FVector> PositionKeys;
	JAnimationKeyList<FVector> RotationKeys;
	JAnimationKeyList<FVector> ScaleKeys;
	class JKeyFrame*           SourceFrame;
};

struct JKeyFrame
{
	JText                  Name;
	JAnimDataTransform     Transform;
	JArray<Ptr<JKeyFrame>> ChildFrames;
};
//
// class JAnimation
// {
// public:
// 	JAnimation();
// 	~JAnimation();
//
// 	void AddTrack(JAnimationTrack& Track);
// 	void RemoveTrack(JAnimationTrack& Track);
// 	void RemoveTrack(uint32_t Index);
// 	void RemoveAllTracks();
//
// 	void SetStartTime(float Time);
// 	void SetEndTime(float Time);
// 	void SetSourceSamplingInterval(float Interval);
//
// 	float GetStartTime() const;
// 	float GetEndTime() const;
// 	float GetSourceSamplingInterval() const;
//
// 	JArray<JAnimationTrack>& GetTracks();
//
// 	void Evaluate(float Time, JArray<JAnimDataTransform>& OutTransforms);
//
// protected:
// 	JText                   mName;
// 	float                   mStartTime;
// 	float                   mEndTime;
// 	float                   mSourceSamplingInterval;
// 	JArray<JAnimationTrack> mTracks;
//
// 	friend class Utils::Fbx::FbxFile;
// };
