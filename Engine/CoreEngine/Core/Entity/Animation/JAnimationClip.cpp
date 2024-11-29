#include "JAnimationClip.h"


void JAnimBoneTrack::AddKey(float InTime, const FVector& InPosition, const FVector4& InRotation, const FVector& InScale)
{
	TransformKeys.PositionKeys.push_back({InTime, InPosition});
	TransformKeys.RotationKeys.push_back({InTime, InRotation});
	TransformKeys.ScaleKeys.push_back({InTime, InScale});
}


void JAnimBoneTrack::OptimizeKeys()
{
	SortKeys();

}

void JAnimBoneTrack::OptimizeKey()
{
	if (TransformKeys.PositionKeys.size() < 2)
	{
		return;
	}

	FVector               currentSlope = FVector::ZeroVector;
	JAnimKeyList<FVector> optimizedPositionKeys;
	optimizedPositionKeys.push_back(TransformKeys.PositionKeys[0]);

	for (int32_t i = 1; i < TransformKeys.PositionKeys.size(); ++i)
	{
		FVector slope = ComputeSlope(TransformKeys.PositionKeys[i - 1], TransformKeys.PositionKeys[i]);
	}
}

void JAnimBoneTrack::SortKeys()
{
	std::ranges::sort(TransformKeys.PositionKeys, LessEqual<FVector>);
	std::ranges::sort(TransformKeys.RotationKeys, LessEqual<FVector4>);
	std::ranges::sort(TransformKeys.ScaleKeys, LessEqual<FVector>);
}

void JAnimBoneTrack::EndianSwap() {}

bool JAnimBoneTrack::IsTrackEmpty() const
{
	return false;
}

bool JAnimationClip::Serialize_Implement(std::ofstream& FileStream)
{
	if (!Utils::Serialization::SerializeMetaData(FileStream, this))
	{
		return false;
	}

	// Name
	Utils::Serialization::Serialize_Text(mName, FileStream);

	// Settings
	Utils::Serialization::Serialize_Primitive(&mStartTime, sizeof(mStartTime), FileStream);
	Utils::Serialization::Serialize_Primitive(&mEndTime, sizeof(mEndTime), FileStream);
	Utils::Serialization::Serialize_Primitive(&mSourceSamplingInterval, sizeof(mSourceSamplingInterval), FileStream);

	// Editor Settings
	Utils::Serialization::Serialize_Primitive(&bLooping, sizeof(bLooping), FileStream);
	Utils::Serialization::Serialize_Primitive(&bPlaying, sizeof(bPlaying), FileStream);
	Utils::Serialization::Serialize_Primitive(&bRootMotion, sizeof(bRootMotion), FileStream);
	Utils::Serialization::Serialize_Primitive(&mElapsedTime, sizeof(mElapsedTime), FileStream);

	// Tracks
	int32_t trackSize = mTracks.size();
	Utils::Serialization::Serialize_Primitive(&trackSize, sizeof(trackSize), FileStream);
	for (int32_t i = 0; i < trackSize; ++i)
	{
		auto track = mTracks[i];

		// Track Name
		Utils::Serialization::Serialize_Text(track->Name, FileStream);

		// Transform Keys
		int32_t keySize = track->TransformKeys.PositionKeys.size();
		Utils::Serialization::Serialize_Primitive(&keySize, sizeof(keySize), FileStream);
		for (int32_t j = 0; j < keySize; ++j)
		{
			Utils::Serialization::Serialize_Primitive(&track->TransformKeys.PositionKeys[j].Time,
													  sizeof(float),
													  FileStream);
			Utils::Serialization::Serialize_Primitive(&track->TransformKeys.PositionKeys[j].Value,
													  sizeof(FVector),
													  FileStream);
			Utils::Serialization::Serialize_Primitive(&track->TransformKeys.RotationKeys[j].Time,
													  sizeof(float),
													  FileStream);
			Utils::Serialization::Serialize_Primitive(&track->TransformKeys.RotationKeys[j].Value,
													  sizeof(FVector4),
													  FileStream);
			Utils::Serialization::Serialize_Primitive(&track->TransformKeys.ScaleKeys[j].Time, sizeof(float), FileStream);
			Utils::Serialization::Serialize_Primitive(&track->TransformKeys.ScaleKeys[j].Value,
													  sizeof(FVector),
													  FileStream);
		}

		// Curve Keys
		int32_t curveSize = track->CurveData.size();
		Utils::Serialization::Serialize_Primitive(&curveSize, sizeof(curveSize), FileStream);
		for (int32_t j = 0; j < curveSize; ++j)
		{
			JAnimKeyList<int32_t> curve = track->CurveData[j];

			int32_t curveKeySize = curve.size();
			Utils::Serialization::Serialize_Primitive(&curveKeySize, sizeof(curveKeySize), FileStream);
			for (int32_t k = 0; k < curveKeySize; ++k)
			{
				Utils::Serialization::Serialize_Primitive(&curve[k].Time, sizeof(float), FileStream);
				Utils::Serialization::Serialize_Primitive(&curve[k].Value, sizeof(int32_t), FileStream);
			}
		}
	}
	return true;
}

bool JAnimationClip::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, GetType()))
	{
		return false;
	}

	// Name
	Utils::Serialization::DeSerialize_Text(mName, InFileStream);

	// Settings
	Utils::Serialization::DeSerialize_Primitive(&mStartTime, sizeof(mStartTime), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&mEndTime, sizeof(mEndTime), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&mSourceSamplingInterval, sizeof(mSourceSamplingInterval), InFileStream);

	// Editor Settings
	Utils::Serialization::DeSerialize_Primitive(&bLooping, sizeof(bLooping), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&bPlaying, sizeof(bPlaying), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&bRootMotion, sizeof(bRootMotion), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&mElapsedTime, sizeof(mElapsedTime), InFileStream);

	// Tracks
	int32_t trackSize;
	Utils::Serialization::DeSerialize_Primitive(&trackSize, sizeof(trackSize), InFileStream);
	for (int32_t i = 0; i < trackSize; ++i)
	{
		auto track = MakePtr<JAnimBoneTrack>();

		// Track Name
		Utils::Serialization::DeSerialize_Text(track->Name, InFileStream);

		// Transform Keys
		int32_t keySize;
		Utils::Serialization::DeSerialize_Primitive(&keySize, sizeof(keySize), InFileStream);
		for (int32_t j = 0; j < keySize; ++j)
		{
			JAnimKey<FVector> positionKey;
			Utils::Serialization::DeSerialize_Primitive(&positionKey.Time, sizeof(float), InFileStream);
			Utils::Serialization::DeSerialize_Primitive(&positionKey.Value, sizeof(FVector), InFileStream);

			JAnimKey<FVector4> orientationKey;
			Utils::Serialization::DeSerialize_Primitive(&orientationKey.Time, sizeof(float), InFileStream);
			Utils::Serialization::DeSerialize_Primitive(&orientationKey.Value, sizeof(FVector4), InFileStream);

			JAnimKey<FVector> scaleKey;
			Utils::Serialization::DeSerialize_Primitive(&scaleKey.Time, sizeof(float), InFileStream);
			Utils::Serialization::DeSerialize_Primitive(&scaleKey.Value, sizeof(FVector), InFileStream);

			track->TransformKeys.PositionKeys.push_back(positionKey);
			track->TransformKeys.RotationKeys.push_back(orientationKey);
			track->TransformKeys.ScaleKeys.push_back(scaleKey);
		}
		mTracks.push_back(track);

		// Curve Keys
		int32_t curveSize;
		Utils::Serialization::DeSerialize_Primitive(&curveSize, sizeof(curveSize), InFileStream);
		for (int32_t j = 0; j < curveSize; ++j)
		{
			JAnimKeyList<int32_t> curve;
			int32_t               curveKeySize;
			Utils::Serialization::DeSerialize_Primitive(&curveKeySize, sizeof(curveKeySize), InFileStream);
			for (int32_t k = 0; k < curveKeySize; ++k)
			{
				JAnimKey<int32_t> curveKey;
				Utils::Serialization::DeSerialize_Primitive(&curveKey.Time, sizeof(float), InFileStream);
				Utils::Serialization::DeSerialize_Primitive(&curveKey.Value, sizeof(int32_t), InFileStream);
				curve.push_back(curveKey);

				return true;
			}
		}
	}

	return true;
}

void JAnimationClip::Play()
{
	mElapsedTime = mStartTime * mFramePerSecond * mTickPerFrame;
	bPlaying     = true;
	bLooping     = true;
}

void JAnimationClip::Resume()
{
	bPlaying = true;
}

void JAnimationClip::Pause()
{
	bPlaying = false;
}

void JAnimationClip::Stop()
{
	bPlaying     = false;
	mElapsedTime = 0.0f;
}

void JAnimationClip::TickAnim(const float DeltaSeconds)
{
	if (!bPlaying || mSkeletalMesh.expired())
	{
		return;
	}

	// 경과시간 계산
	mElapsedTime += DeltaSeconds * mFramePerSecond * mTickPerFrame * mAnimationSpeed;

	// 경과 시간이 애니메이션의 시작 시간을 초과
	if (mElapsedTime >= mEndTime * mFramePerSecond * mTickPerFrame)
	{
		if (!bLooping)
		{
			Stop();
			return;
		}

		// 시작 시간으로 초기화
		mElapsedTime = mStartTime * mFramePerSecond * mTickPerFrame;
	}

	const auto& bones = mSkeletalMesh.lock()->GetSkeletonData().Joints;

	for (int32_t boneIndex = 0; boneIndex < bones.size(); ++boneIndex)
	{
		const auto& boneParentIndex = bones[boneIndex].ParentIndex;

		FMatrix parentAnimPose = boneParentIndex == -1
									 ? FMatrix::Identity
									 : mAnimationPose[boneParentIndex];

		const FMatrix thisFramePose = FetchInterpolateBone(boneIndex, parentAnimPose, mElapsedTime);

		mAnimationPose[boneIndex] = thisFramePose;
	}
	if (!bInterpolate)
	{
		bInterpolate = true;
	}

}

void JAnimationClip::OptimizeKeys()
{
	JArray<Ptr<JAnimBoneTrack>> optimizedTracks;
	for (int32_t i = 0; i < mTracks.size(); ++i)
	{
		auto track = mTracks[i];

		track->OptimizeKeys();
		if (track->IsTrackEmpty())
		{
			// 트랙이 비어있으면 삭제 예약
			track = nullptr;
		}
		else
		{
			optimizedTracks.push_back(track);
		}
	}

	mTracks = optimizedTracks;
}

void JAnimationClip::AddTrack(const Ptr<JAnimBoneTrack>& Track)
{
	mTracks.push_back(Track);
}

void JAnimationClip::RemoveTrack(uint32_t Index)
{
	if (mTracks.size() > Index)
	{
		mTracks.erase(mTracks.begin() + Index);
	}
	else
	{
		LOG_CORE_ERROR(" Index out of range in JAnimationClip::RemoveTrack");
	}
}

void JAnimationClip::RemoveAllTracks() { mTracks.clear(); }

FMatrix JAnimationClip::FetchInterpolateBone(const int32_t  InBoneIndex,
											 const FMatrix& InParentBoneMatrix,
											 const float    InAnimElapsedTime) const
{
	FVector     position       = FVector::ZeroVector;
	FVector     cachedPosition = FVector::ZeroVector;
	FQuaternion rotation       = FQuaternion::Identity;
	FQuaternion cachedRotation = FQuaternion::Identity;
	FVector     scale          = FVector::OneVector;
	FVector     cachedScale    = FVector::OneVector;

	FMatrix positionMat = FMatrix::Identity;
	FMatrix rotationMat = FMatrix::Identity;
	FMatrix scaleMat    = FMatrix::Identity;

	float startTick = mStartTime * mFramePerSecond * mTickPerFrame;
	float endTick   = 0.f;

	const auto boneTrack = mTracks[InBoneIndex];

	{
		const auto cachedPose = mAnimationPose[InBoneIndex];
		XMVECTOR   cachedPosition_Local, cachedRotation_Local, cachedScale_Local;
		XMMatrixDecompose(&cachedScale_Local, &cachedRotation_Local, &cachedPosition_Local, cachedPose);

		cachedPosition = cachedPosition_Local;
		cachedRotation = cachedRotation_Local;
		cachedScale    = cachedScale_Local;
	}


	if (!boneTrack)
	{
		LOG_CORE_ERROR("Invalid Bone Track in JAnimationClip::FetchInterpolateBone");
		return FMatrix::Identity;
	}

	JAnimKey<FVector> startTrackPosition;
	JAnimKey<FVector> endTrackPosition;
	bool              bShouldInterpolate = FindDeltaKey<FVector>(boneTrack->TransformKeys.PositionKeys,
																 InAnimElapsedTime,
																 startTrackPosition,
																 endTrackPosition);

	position  = startTrackPosition.Value;
	startTick = startTrackPosition.Time * mFramePerSecond * mTickPerFrame;
	if (bShouldInterpolate)
	{
		endTick  = endTrackPosition.Time * mFramePerSecond * mTickPerFrame;
		position = XMVectorLerp(position,
								endTrackPosition.Value,
								(InAnimElapsedTime - startTick) / (endTick - startTick));
	}


	JAnimKey<FVector4> startTrackRotation;
	JAnimKey<FVector4> endTrackRotation;
	bShouldInterpolate = FindDeltaKey<FVector4>(boneTrack->TransformKeys.RotationKeys,
												InAnimElapsedTime,
												startTrackRotation,
												endTrackRotation);

	rotation  = FQuaternion{startTrackRotation.Value};
	startTick = startTrackRotation.Time * mFramePerSecond * mTickPerFrame;
	if (bShouldInterpolate)
	{
		endTick  = endTrackRotation.Time * mFramePerSecond * mTickPerFrame;
		rotation = FQuaternion::Slerp(rotation,
									  FQuaternion{endTrackRotation.Value},
									  (InAnimElapsedTime - startTick) / (endTick - startTick));
	}

	JAnimKey<FVector> startTrackScale;
	JAnimKey<FVector> endTrackScale;
	bShouldInterpolate = FindDeltaKey<FVector>(boneTrack->TransformKeys.ScaleKeys,
											   InAnimElapsedTime,
											   startTrackScale,
											   endTrackScale);
	scale     = startTrackScale.Value;
	startTick = startTrackScale.Time * mFramePerSecond * mTickPerFrame;
	if (bShouldInterpolate)
	{
		endTick = endTrackScale.Time * mFramePerSecond * mTickPerFrame;
		scale   = XMVectorLerp(scale,
							   endTrackScale.Value,
							   (InAnimElapsedTime - startTick) / (endTick - startTick));
	}

	// if (bInterpolate)
	// {
	// 	position = XMVectorLerp(cachedPosition, position, 0.5f);
	// 	rotation = FQuaternion::Slerp(cachedRotation, rotation, 0.5f);
	// 	scale    = XMVectorLerp(cachedScale, scale, 0.5f);
	// }

	positionMat = XMMatrixTranslation(position.x, position.y, position.z);
	XMStoreFloat4x4(&rotationMat, XMMatrixRotationQuaternion(rotation));
	scaleMat = XMMatrixScaling(scale.x, scale.y, scale.z);

	FMatrix finalMatrix = scaleMat * rotationMat * positionMat;
	finalMatrix         = finalMatrix * InParentBoneMatrix;

	return finalMatrix;
}

void JAnimationClip::SetSkeletalMesh(const Ptr<JSkeletalMesh>& InSkeletalMesh)
{
	mSkeletalMesh = InSkeletalMesh;
	mAnimationPose.clear();

	for (int32_t i = 0; i < InSkeletalMesh->GetSkeletonData().Joints.size(); ++i)
	{
		mAnimationPose.push_back(FMatrix::Identity);
	}
}
