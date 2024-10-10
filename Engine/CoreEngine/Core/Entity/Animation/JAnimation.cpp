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

void JAnimationTrack::AddKey(float InTime, const FVector& InPosition, const FVector& InRotation, const FVector& InScale)
{
	TransformKeys.PositionKeys.push_back({InTime, InPosition});
	TransformKeys.RotationKeys.push_back({InTime, InRotation});
	TransformKeys.ScaleKeys.push_back({InTime, InScale});
}
