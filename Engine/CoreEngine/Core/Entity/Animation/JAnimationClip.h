#pragma once
#include "common_include.h"
#include "Core/Graphics/Mesh/JMeshData.h"
#include "Core/Utils/FileIO/JSerialization.h"
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
struct JAnimKey
{
	float Time;
	T     Value;
};

/** 키 데이터 배열 */
template <typename T>
using JAnimKeyList = JArray<JAnimKey<T>>;

/** 본 트랙에서 보간할 키 프레임 구간을 찾아보자 */
template <typename T>
bool FindDeltaKey(const JAnimKeyList<T>& InKeys, const float InTime, JAnimKey<T>& OutStartTrack, JAnimKey<T>& OutEndTrack)
{
	for (int32_t i = 0; i < InKeys.size(); ++i)
	{
		if (InKeys[i].Time * 30.f * 160.f >= InTime)
		{
			OutEndTrack = InKeys[i];
			return true;
		}
		OutStartTrack = InKeys[i];
	}

	return false;
}

/**
 * 변환 정보를 가지는 키 프레임 데이터
 */
struct FAnimKeyDataTransform
{
	JAnimKeyList<FVector>  PositionKeys;
	JAnimKeyList<FVector4> RotationKeys;
	JAnimKeyList<FVector>  ScaleKeys;
};


struct JAnimBoneTrack
{
	void AddKey(float InTime, const FVector& InPosition, const FVector4& InRotation, const FVector& InScale);

	void OptimizeKeys();
	void OptimizeKey();
	void SortKeys();
	void EndianSwap();

	bool IsTrackEmpty() const;

	JText                 Name;				// 본(조인트) 이름
	FAnimKeyDataTransform TransformKeys;	// 변환 행렬 데이터


	/// Curve?
	/// 언리얼 애니메이션에서 프레임 마다 특정 값을 부여할 수 있다.
	/// 이 값을 가지고 사용자가 다양하게 사용할 수 있음
	/// TODO: 보간 알고리즘을 사용하여 값을 계산할 수 있도록 구현해야 함
	JArray<JAnimKeyList<int32_t>> CurveData;	// 애니메이션 커브 데이터
};


class JAnimationClip : public ISerializable
{
public:
	JAnimationClip();
	~JAnimationClip() override;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Play();
	void Resume();
	void Pause();
	void Stop();

	void TickAnim(const float DeltaSeconds);

public:
	void OptimizeKeys();
	void AddTrack(const Ptr<JAnimBoneTrack>& Track);
	void RemoveTrack(uint32_t Index);
	void RemoveAllTracks();

public:
	[[nodiscard]] FMatrix FetchInterpolateBone(const int32_t  InBoneIndex,
											   const FMatrix& InParentBoneMatrix,
											   const float    InAnimElapsedTime) const;

public:
	[[nodiscard]] FORCEINLINE float GetStartTime() const { return mStartTime; }
	[[nodiscard]] FORCEINLINE float GetEndTime() const { return mEndTime; }
	[[nodiscard]] FORCEINLINE float GetSourceSamplingInterval() const { return mSourceSamplingInterval; }
	[[nodiscard]] FORCEINLINE JArray<Ptr<JAnimBoneTrack>>& GetTracks() { return mTracks; }
	[[nodiscard]] FORCEINLINE const JArray<FMatrix>& GetAnimPoses() { return mAnimationPose; }

	void SetSkeletalMesh(const Ptr<class JSkeletalMesh>& InSkeletalMesh);

protected:
	// ------------ Properties ------------
	JText mName;						// 애니메이션 클립 이름

	// ------------ ReadOnly Variable (기본 값) ------------
	float   mStartTime;					// 시작 시간
	float   mEndTime;					// 끝 시간
	float   mSourceSamplingInterval;	// 샘플링 간격
	int32_t mStartFrame; 				// 시작 프레임
	int32_t mEndFrame;					// 끝 프레임

	float mAnimationSpeed = 1.f;	// 애니메이션 속도
	float mFramePerSecond = 30.f;	// 초당 프레임 수
	float mTickPerFrame   = 160.f;	// 프레임당 틱 수

	// ------------ Editable Variable ------------
	bool  bLooping;					// 루프 여부
	bool  bPlaying;					// 재생 여부
	bool  bRootMotion;				// 루트 모션 여부 (어떻게 처리할지 고민해봐야 함 자연스러운 공격 모션을 위해선 필수적)
	float mElapsedTime;				// 경과 시간

	// ------------ Skeletal Mesh Data ------------
	WPtr<JSkeletalMesh> mSkeletalMesh;	// 스켈레탈 메쉬

	// ------------ Animation Data ------------
	JArray<Ptr<JAnimBoneTrack>> mTracks;		// 본별로 트랙을 가지고 있는 배열
	JArray<FMatrix>             mAnimationPose;	// 애니메이션 포즈

	bool bInterpolate = false;

	friend class Utils::Fbx::FbxFile;
};

template <typename T>
bool LessEqual(const JAnimKey<T> InKeyA, const JAnimKey<T> InKeyB)
{
	return InKeyA.Time <= InKeyB.Time;
}


template <typename T>
FVector ComputeSlope(const JAnimKey<T>& InKeyA, const JAnimKey<T>& InKeyB)
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
