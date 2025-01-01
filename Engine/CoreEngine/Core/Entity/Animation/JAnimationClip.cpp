#include "JAnimationClip.h"
#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Component/Mesh/JSkeletalMeshComponent.h"


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

void JAnimBoneTrack::EndianSwap()
{}

bool JAnimBoneTrack::IsTrackEmpty() const
{
	return false;
}

JAnimationClip::JAnimationClip(JTextView InName, JSkeletalMesh* InSkeletalMesh)
	: mName(InName),
	  mStartTime(0),
	  mEndTime(0),
	  mSourceSamplingInterval(0),
	  mStartFrame(0),
	  mEndFrame(0),
	  bLooping(false),
	  bPlaying(false),
	  bRootMotion(false),
	  mElapsedTime(0)
{
	mSkeletalMesh = InSkeletalMesh;
}

JAnimationClip::JAnimationClip(JTextView InName, JSkeletalMeshComponent* InSkeletalMesh)
	: mName(InName),
	  mStartTime(0),
	  mEndTime(0),
	  mSourceSamplingInterval(0),
	  mStartFrame(0),
	  mEndFrame(0),
	  bLooping(false),
	  bPlaying(false),
	  bRootMotion(false),
	  mElapsedTime(0)
{
	mSkeletalMeshComponent = InSkeletalMesh;
	mSkeletalMesh          = InSkeletalMesh->GetSkeletalMesh();
}

JAnimationClip::JAnimationClip(const JAnimationClip& InOther)
	: mName(InOther.mName),
	  mStartTime(InOther.mStartTime),
	  mEndTime(InOther.mEndTime),
	  mSourceSamplingInterval(InOther.mSourceSamplingInterval),
	  mStartFrame(InOther.mStartFrame),
	  mEndFrame(InOther.mEndFrame),
	  bLooping(InOther.bLooping),
	  bPlaying(InOther.bPlaying),
	  bRootMotion(InOther.bRootMotion),
	  mTracks(InOther.mTracks),
	  mBoneMatrix(InOther.mBoneMatrix),
	  mInstanceData(InOther.mInstanceData),
	  mEvents(InOther.mEvents)
{
	OnAnimStart    = InOther.OnAnimStart;
	OnAnimFinished = InOther.OnAnimFinished;
	OnAnimBlendOut = InOther.OnAnimBlendOut;
	mSkeletalMesh  = InOther.mSkeletalMesh;
}

UPtr<IManagedInterface> JAnimationClip::Clone() const
{
	return MakeUPtr<JAnimationClip>(*this);
}

uint32_t JAnimationClip::GetHash() const
{
	return 0;
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
	Utils::Serialization::Serialize_Primitive(&mStartFrame, sizeof(mStartFrame), FileStream);
	Utils::Serialization::Serialize_Primitive(&mEndFrame, sizeof(mEndFrame), FileStream);
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
			Utils::Serialization::Serialize_Primitive(&track->TransformKeys.ScaleKeys[j].Time,
													  sizeof(float),
													  FileStream);
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

		// int32_t stateSize = mTransitionMap.size();
		// Utils::Serialization::Serialize_Primitive(&stateSize, sizeof(stateSize), FileStream);
		// for (auto& [key, value] : mTransitionMap)
		// {
		// 	JText keyStr       = key;
		// 	Utils::Serialization::Serialize_Text(keyStr, FileStream);
		// 	// Utils::Serialization::Serialize_Text(animClipName, FileStream);
		// }
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
	Utils::Serialization::DeSerialize_Primitive(&mStartFrame, sizeof(mStartFrame), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&mEndFrame, sizeof(mEndFrame), InFileStream);
	Utils::Serialization::DeSerialize_Primitive(&mSourceSamplingInterval,
												sizeof(mSourceSamplingInterval),
												InFileStream);

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

void JAnimationClip::Initialize()
{
	mEvents[0].Bind([this](){ OnAnimStart.Execute(); });
	mEvents[mEndFrame - 1].Bind([this](){ OnAnimFinished.Execute(); });
	OnAnimBlendOut.Bind([&](){
		if (bRootMotion)
		{
			ApplyRootMotion();
		}
	});
}

void JAnimationClip::Play()
{
	mElapsedTime = mStartTime * mFramePerSecond * mTickPerFrame;
	bPlaying     = true;
	bLooping     = true;
	mNextState   = "";
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

void JAnimationClip::ApplyRootMotion() const
{
	mSkeletalMeshComponent->GetOwnerActor()->AddLocalLocation(mRootMotionValue.Position);
}

bool JAnimationClip::TickAnim(const float DeltaSeconds)
{
	if (!bPlaying || !mSkeletalMesh)
	{
		return true;
	}

	// 경과시간 계산
	mElapsedTime += DeltaSeconds * mAnimationSpeed;

	// 경과 시간이 애니메이션의 시작 시간을 초과
	if (mElapsedTime >= mEndTime)
	{
		if (!bLooping)
		{
			Stop();
			return true;
		}
		
		mElapsedTime = mStartTime + DeltaSeconds * mAnimationSpeed;
		OnAnimBlendOut.Execute();
	}

	UpdateInstanceData(mElapsedTime);

	mEvents[mInstanceData.CurrentAnimIndex].Execute();

	if (CheckTransition())
	{
		return false;
	}


	return true;
}

bool JAnimationClip::CheckTransition()
{
	for (auto& transition : mTransitionMap)
	{
		JText nextState      = transition.first;
		float transitionTime = transition.second[0].TransitionTime;

		if (transition.second[0].Condition())
		{
			mNextState = nextState;
			OnAnimBlendOut.Execute();

			return true;
		}
	}

	return false;
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

void JAnimationClip::SetLoop(bool bEnableLoop)
{
	bLooping = bEnableLoop;
}

void JAnimationClip::SetRootMotion(bool bEnableRootMotion)
{
	bRootMotion = bEnableRootMotion;
}

FMatrix JAnimationClip::FetchInterpolateBone(const int32_t  InBoneIndex,
											 const FMatrix& InParentBoneMatrix,
											 const float    InAnimElapsedTime) const
{
	FVector     position = FVector::ZeroVector;
	FQuaternion rotation = FQuaternion::Identity;
	FVector     scale    = FVector::OneVector;

	FMatrix positionMat = FMatrix::Identity;
	FMatrix rotationMat = FMatrix::Identity;
	FMatrix scaleMat    = FMatrix::Identity;

	float startTick = mStartTime;
	float endTick   = 0.f;

	const auto boneTrack = mTracks[InBoneIndex];

	if (!boneTrack)
	{
		LOG_CORE_ERROR("Invalid Bone Track in JAnimationClip::FetchInterpolateBone");
		return FMatrix::Identity;
	}

	int32_t startFrame = 0, endFrame = 0;

	FindFrame(boneTrack->TransformKeys.PositionKeys,
			  InAnimElapsedTime,
			  startFrame,
			  endFrame);


	position = boneTrack->TransformKeys.PositionKeys[startFrame].Value;
	// if (startFrame != endFrame)
	// {
	// 	startTick = boneTrack->TransformKeys.PositionKeys[startFrame].Time * mFramePerSecond * mTickPerFrame;
	// 	endTick   = boneTrack->TransformKeys.PositionKeys[endFrame].Time * mFramePerSecond * mTickPerFrame;
	// 	position  = XMVectorLerp(position,
	// 							 boneTrack->TransformKeys.PositionKeys[endFrame].Value,
	// 							 (InAnimElapsedTime - startTick) / (endTick - startTick));
	// }


	rotation = FQuaternion{boneTrack->TransformKeys.RotationKeys[startFrame].Value};
	// if (startFrame != endFrame)
	// {
	// 	rotation = FQuaternion::Slerp(rotation,
	// 								  FQuaternion{boneTrack->TransformKeys.RotationKeys[endFrame].Value},
	// 								  (InAnimElapsedTime - startTick) / (endTick - startTick));
	//
	// }

	scale = boneTrack->TransformKeys.ScaleKeys[startFrame].Value;
	// if (startFrame != endFrame)
	// {
	// 	scale = XMVectorLerp(scale,
	// 						 boneTrack->TransformKeys.ScaleKeys[endFrame].Value,
	// 						 (InAnimElapsedTime - startTick) / (endTick - startTick));
	// }


	positionMat = XMMatrixTranslation(position.x, position.y, position.z);
	XMStoreFloat4x4(&rotationMat, XMMatrixRotationQuaternion(rotation));
	scaleMat = XMMatrixScaling(scale.x, scale.y, scale.z);

	// FMatrix localTransform = FMatrix::CreateFromScaleRotationTranslation(scale,
	// 																	 rotation,
	// 																	 position
	// 																	);

	FMatrix finalMatrix = scaleMat * rotationMat * positionMat;
	finalMatrix         = finalMatrix * InParentBoneMatrix;

	return finalMatrix;
}

FMatrix JAnimationClip::GetInterpolatedBone(const JText& InBoneName)
{
	auto it = mBoneMatrix.find(InBoneName);
	if (it == mBoneMatrix.end())
		return FMatrix::Identity;

	auto& boneMatrix = it->second;

	if (mInstanceData.CurrentAnimIndex >= boneMatrix.size())
	{
		mInstanceData.CurrentAnimIndex = 0;
	}

	if (mInstanceData.NextAnimIndex >= boneMatrix.size())
	{
		mInstanceData.NextAnimIndex = 0;
	}

	return FMatrix::Lerp(boneMatrix[mInstanceData.CurrentAnimIndex],
						 boneMatrix[mInstanceData.NextAnimIndex],
						 mInstanceData.DeltaTime);
}

void JAnimationClip::UpdateInstanceData(const float InAnimElapsedTime)
{
	const auto boneTrack = mTracks[0];

	if (!boneTrack)
	{
		LOG_CORE_ERROR("Invalid Bone Track in JAnimationClip::FetchInterpolateBone");
		return;
	}

	int32_t startFrame = 0, endFrame = 0;

	float startTick;
	float endTick;

	if (FindFrame(boneTrack->TransformKeys.PositionKeys, InAnimElapsedTime, startFrame, endFrame))
	{
		// 정상적으로 startFrame과 endFrame을 찾은 경우
		startTick = boneTrack->TransformKeys.PositionKeys[startFrame].Time;
		endTick   = boneTrack->TransformKeys.PositionKeys[endFrame].Time;


		mInstanceData.DeltaTime        = (InAnimElapsedTime - startTick) / (endTick - startTick);
		mInstanceData.CurrentAnimIndex = startFrame;
		mInstanceData.NextAnimIndex    = endFrame;
	}
	if (startFrame >= mEndFrame)
	{
		mInstanceData.DeltaTime        = (0.33);
		mInstanceData.CurrentAnimIndex = 0;
		mInstanceData.NextAnimIndex    = 0;
	}
}

void JAnimationClip::AddTransition(const JText& InState, const std::function<bool()>& InFunc, float InTransitionTime)
{
	mTransitionMap[InState].emplace_back(InFunc, InTransitionTime);
}


void JAnimationClip::SetSkeletalMesh(JSkeletalMesh* InSkeletalMesh)
{
	mSkeletalMesh = InSkeletalMesh;
}

float JAnimationClip::GetElapsedRatio() const
{
	return mElapsedTime / mEndTime;
}

uint32_t JAnimationClip::GenerateAnimationTexture(JArray<FVector4>& OutTextureData)
{
	if (!mSkeletalMesh)
	{
		LOG_CORE_WARN("Skeletal Mesh is not initialized in JAnimationClip::GenerateAnimationTexture");
		return 0;
	}

	const auto&   skeleton   = mSkeletalMesh->GetSkeletonData();
	const auto&   skin       = mSkeletalMesh->GetSkinData();
	const int32_t boneCount  = mSkeletalMesh->GetSkeletonData().Joints.size();
	const int32_t frameCount = mEndFrame - mStartFrame;

	JArray<FMatrix> boneWorldTransforms(boneCount); // 현재 프레임의 월드 변환 행렬

	const uint32_t offset = OutTextureData.size();

	// 데이터 채우기
	for (int32_t frameIndex = 0; frameIndex < frameCount; ++frameIndex)
	{
		for (int32_t boneIndex = 0; boneIndex < boneCount; ++boneIndex)
		{
			const int32_t parentIndex = skeleton.Joints[boneIndex].ParentIndex;

			// 로컬 변환 가져오기
			const auto& positionKey = mTracks[boneIndex]->TransformKeys.PositionKeys[frameIndex];
			const auto& rotationKey = mTracks[boneIndex]->TransformKeys.RotationKeys[frameIndex];
			const auto& scaleKey    = mTracks[boneIndex]->TransformKeys.ScaleKeys[frameIndex];


			FMatrix positionMat;
			FMatrix rotationMat;
			FMatrix scaleMat;
			positionMat = XMMatrixTranslation(positionKey.Value.x, positionKey.Value.y, positionKey.Value.z);
			XMStoreFloat4x4(&rotationMat, XMMatrixRotationQuaternion(rotationKey.Value));
			scaleMat = XMMatrixScaling(scaleKey.Value.x, scaleKey.Value.y, scaleKey.Value.z);

			FMatrix localTransform = scaleMat * rotationMat * positionMat;

			// 부모 월드 변환 적용
			if (parentIndex == -1)
			{
				boneWorldTransforms[boneIndex] = localTransform; // 루트 본
			}
			else
			{
				boneWorldTransforms[boneIndex] = localTransform * boneWorldTransforms[parentIndex];
			}

			// 최종 월드 변환 데이터 추가
			const FMatrix& worldTransform = skin->GetInfluenceBoneInverseBindPose(skin->GetInfluenceBoneName(boneIndex))
					*
					boneWorldTransforms[boneIndex];

			OutTextureData.emplace_back(FVector4{worldTransform.m[0]});
			OutTextureData.emplace_back(FVector4{worldTransform.m[1]});
			OutTextureData.emplace_back(FVector4{worldTransform.m[2]});
			OutTextureData.emplace_back(FVector4{worldTransform.m[3]});

			mBoneMatrix[skin->GetInfluenceBoneName(boneIndex)].push_back(worldTransform);
		}
	}

	mAnimOffset                     = offset;
	mInstanceData.CurrentAnimOffset = mAnimOffset;
	mInstanceData.NextAnimOffset    = mAnimOffset;
	mInstanceData.BoneCount         = boneCount;

	mEvents.resize(mEndFrame);


	FVector initialTransform;
	FVector finalTransform;
	initialTransform = mTracks[0]->TransformKeys.PositionKeys[0].Value;
	finalTransform   = mTracks[0]->TransformKeys.PositionKeys[mEndFrame - 1].Value;

	// 루트 모션 계산
	FVector deltaTransform = finalTransform - initialTransform;

	// deltaTransform에서 위치, 회전, 스케일을 분리하여 루트 모션 값으로 저장
	// FVector     deltaPosition(deltaTransform.m[3][0], deltaTransform.m[3][1], deltaTransform.m[3][2]);
	FQuaternion deltaRotation;
	// 루트 모션 값 설정
	mRootMotionValue = {deltaTransform, deltaRotation};

	return offset;
}
