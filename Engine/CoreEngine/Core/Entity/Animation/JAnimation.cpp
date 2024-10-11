#include "JAnimation.h"

void JAnimationTrack::AddKey(float InTime, const JAnimDataTransform& InTransformData)
{
	TransformKeys.PositionKeys.push_back({InTime, InTransformData.Position});
	TransformKeys.RotationKeys.push_back({InTime, InTransformData.Rotation});
	TransformKeys.ScaleKeys.push_back({InTime, InTransformData.Scale});
}

void JAnimationTrack::AddKey(float InTime, const FMatrix& InTransformMatrix)
{
	DirectX::XMVECTOR position, rotation, scale;

	XMMatrixDecompose(&scale, &rotation, &position, DirectX::XMLoadFloat4x4(&InTransformMatrix));

	// TransformKeys.PositionKeys.push_back({InTime, position.});
}

void JAnimationTrack::AddKey(float InTime, const FVector& InPosition, const FVector4& InRotation, const FVector& InScale)
{
	TransformKeys.PositionKeys.push_back({InTime, InPosition});
	TransformKeys.RotationKeys.push_back({InTime, InRotation});
	TransformKeys.ScaleKeys.push_back({InTime, InScale});
}


void JAnimationTrack::OptimizeKeys()
{}

void JAnimationTrack::OptimizeKey()
{
	if (TransformKeys.PositionKeys.size() < 2)
	{
		return;
	}

	FVector                    currentSlope = FVector::ZeroVector;
	JAnimationKeyList<FVector> optimizedPositionKeys;
	optimizedPositionKeys.push_back(TransformKeys.PositionKeys[0]);

	for (int32_t i = 1; i < TransformKeys.PositionKeys.size(); ++i)
	{
		FVector slope = ComputeSlope(TransformKeys.PositionKeys[i - 1], TransformKeys.PositionKeys[i]);
	}
}

void JAnimationTrack::SortKeys()
{
	std::ranges::sort(TransformKeys.PositionKeys, LessEqual<FVector>);
	std::ranges::sort(TransformKeys.RotationKeys, LessEqual<FVector4>);
	std::ranges::sort(TransformKeys.ScaleKeys, LessEqual<FVector>);
}

void JAnimationTrack::EndianSwap() {}

JAnimation::JAnimation()
{}

JAnimation::~JAnimation()
{}

void JAnimation::AddTrack(const Ptr<JAnimationTrack>& Track)
{
	mTracks.push_back(Track);
}
