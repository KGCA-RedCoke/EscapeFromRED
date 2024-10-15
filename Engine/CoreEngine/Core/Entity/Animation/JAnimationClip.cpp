#include "JAnimationClip.h"


void JAnimBoneTrack::AddKey(float InTime, const FVector& InPosition, const FVector& InRotation, const FVector& InScale)
{
	TransformKeys.PositionKeys.push_back({InTime, InPosition});
	TransformKeys.RotationKeys.push_back({InTime, InRotation});
	TransformKeys.ScaleKeys.push_back({InTime, InScale});
}


void JAnimBoneTrack::OptimizeKeys()
{}

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
	std::ranges::sort(TransformKeys.RotationKeys, LessEqual<FVector>);
	std::ranges::sort(TransformKeys.ScaleKeys, LessEqual<FVector>);
}

void JAnimBoneTrack::EndianSwap() {}

JAnimationClip::JAnimationClip()
{}

JAnimationClip::~JAnimationClip()
{}

uint32_t JAnimationClip::GetType() const
{
	return StringHash("JAnimationClip");
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
													  sizeof(FVector),
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

			JAnimKey<FVector> orientationKey;
			Utils::Serialization::DeSerialize_Primitive(&orientationKey.Time, sizeof(float), InFileStream);
			Utils::Serialization::DeSerialize_Primitive(&orientationKey.Value, sizeof(FVector), InFileStream);

			JAnimKey<FVector> scaleKey;
			Utils::Serialization::DeSerialize_Primitive(&scaleKey.Time, sizeof(float), InFileStream);
			Utils::Serialization::DeSerialize_Primitive(&scaleKey.Value, sizeof(FVector), InFileStream);

			track->TransformKeys.PositionKeys.push_back(positionKey);
			track->TransformKeys.RotationKeys.push_back(orientationKey);
			track->TransformKeys.ScaleKeys.push_back(scaleKey);
		}

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
	bPlaying = true;
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
	if (!bPlaying)
	{
		return;
	}

	// 경과시간 계산
	mElapsedTime += DeltaSeconds * mFramePerSecond * mTickPerFrame;

	// 경과 시간이 애니메이션의 시작 시간을 초과
	if (mElapsedTime >= mEndFrame * mTickPerFrame)
	{
		if (bLooping)
		{
			// 시작 시간으로 초기화
			mElapsedTime = mStartFrame * mTickPerFrame;
		}
		else
		{
			// 끝 시간으로 초기화 및 정지
			mElapsedTime = mEndFrame * mTickPerFrame;
			Stop();
		}
	}

	


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
