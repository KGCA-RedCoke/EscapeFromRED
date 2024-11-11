#include "JSkeletalMeshObject.h"

#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/MManagerInterface.h"

JSkeletalMeshObject::JSkeletalMeshObject(const JText& InName, const std::vector<Ptr<JMeshData>>& InData)
{
	mName                         = InName;
	mVertexSize                   = sizeof(Vertex::FVertexInfo_Base);
	mMeshConstantBuffer.MeshFlags = EnumAsByte(EMeshType::Skeletal);

	if (InData.empty())
	{
		if (!Utils::Serialization::DeSerialize(InName.c_str(), this))
		{
			LOG_CORE_ERROR("Failed to load mesh object(Invalid Path maybe...): {0}", InName);
		}
	}
	else
	{
		mPrimitiveMeshData = InData;
		JSkeletalMeshObject::CreateBuffers();
	}

	assert(mPrimitiveMeshData.size() > 0);

	Ptr<JSkeletalMesh> meshData = std::dynamic_pointer_cast<JSkeletalMesh>(mPrimitiveMeshData[0]);

	mSkeletonData = meshData->GetSkeletonData();
	mSkinData     = meshData->GetSkinData();

	mSampleAnimation = MakePtr<JAnimationClip>();
	if (!Utils::Serialization::DeSerialize("Game/Animation/ThirdPersonWalkFemale_Zombie.jasset", mSampleAnimation.get()))
	{
		LOG_CORE_ERROR("Failed to load animation object(Invalid Path maybe...): {0}", "Game/Animation/Unreal Take.jasset");
	}

	for (int32_t i = 0; i < mSampleAnimation->GetTracks().size(); ++i)
	{
		mCurrentAnimationPose.try_emplace(mSampleAnimation->GetTracks()[i]->Name, FMatrix::Identity);
	}
}

JSkeletalMeshObject::JSkeletalMeshObject(const JWText& InName, const std::vector<Ptr<JMeshData>>& InData)
	: JMeshObject(InName, InData) {}

JSkeletalMeshObject::JSkeletalMeshObject(const JMeshObject& Other)
	: JMeshObject(Other) {}

Ptr<IManagedInterface> JSkeletalMeshObject::Clone() const
{
	auto clone = MakePtr<JSkeletalMeshObject>(*this);
	return clone;
}

void JSkeletalMeshObject::CreateBuffers()
{
	ID3D11Device* device = IManager.RenderManager->GetDevice();
	assert(device);

	JMeshObject::CreateBuffers();

	// Bone 버퍼 생성
	Utils::DX::CreateBuffer(device,
							D3D11_BIND_SHADER_RESOURCE,
							nullptr,
							sizeof(FMatrix),
							SIZE_MAX_BONE_NUM,
							mInstanceBuffer_Bone.Buffer_Bone.GetAddressOf(),
							D3D11_USAGE_DYNAMIC,
							D3D11_CPU_ACCESS_WRITE);

	D3D11_SHADER_RESOURCE_VIEW_DESC boneSRVDesc;
	ZeroMemory(&boneSRVDesc, sizeof(boneSRVDesc));
	{
		boneSRVDesc.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;	// 4x4 Matrix
		boneSRVDesc.ViewDimension        = D3D11_SRV_DIMENSION_BUFFER;		// Buffer
		boneSRVDesc.Buffer.ElementOffset = 0;
		boneSRVDesc.Buffer.ElementWidth  = SIZE_MAX_BONE_NUM * 4;
		CheckResult(device->CreateShaderResourceView(mInstanceBuffer_Bone.Buffer_Bone.Get(),
													 &boneSRVDesc,
													 mInstanceBuffer_Bone.Resource_Bone.GetAddressOf()));
	}

}

void JSkeletalMeshObject::UpdateBoneBuffer(ID3D11DeviceContext* InDeviceContext)
{
	const int32_t   boneCount = mSkinData->GetInfluenceBoneCount();	// 본 개수
	JArray<FMatrix> boneMatrix(boneCount, FMatrix::Identity);	// 셰이더에 넘겨줄 새로운 본 행렬

	// 본 업데이트
	for (int32_t i = 0; i < boneCount; ++i)
	{
		const JText jointName = mSkinData->GetInfluenceBoneName(i);
		FMatrix     newPose   = mSkinData->GetInfluenceBoneInverseBindPose(jointName);

		// 최종 본 변환 행렬 = 본 로컬 변환 행렬 * 본 역바인드 행렬(부모 본의 역바인드 행렬)
		boneMatrix[i] = newPose * mCurrentAnimationPose[
			jointName]; /* * newAnimMatrix (여기에 애니메이션 프레임간의 보간된 본 로컬 변환 행렬을 곱해줌) */
	}

	Utils::DX::UpdateDynamicBuffer(
								   InDeviceContext,
								   mInstanceBuffer_Bone.Buffer_Bone.Get(),
								   boneMatrix.data(),
								   boneCount * sizeof(FMatrix));
}

uint32_t JSkeletalMeshObject::GetType() const
{
	return HASH_ASSET_TYPE_SKELETAL_MESH;
}

bool JSkeletalMeshObject::Serialize_Implement(std::ofstream& FileStream)
{
	bool bSuccess = JMeshObject::Serialize_Implement(FileStream);

	return bSuccess;
}

bool JSkeletalMeshObject::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, this->GetType()))
	{
		return false;
	}

	// Mesh Name
	Utils::Serialization::DeSerialize_Text(mName, InFileStream);

	// Primitive Mesh Data
	int32_t meshDataSize;
	Utils::Serialization::DeSerialize_Primitive(&meshDataSize, sizeof(meshDataSize), InFileStream);

	for (int32_t i = 0; i < meshDataSize; ++i)
	{
		auto archivedData = MakePtr<JSkeletalMesh>();
		archivedData->DeSerialize_Implement(InFileStream);
		mPrimitiveMeshData.push_back(archivedData);
	}

	CreateBuffers();

	return true;
}

void JSkeletalMeshObject::Tick(float DeltaTime)
{
	// 애니메이션 루프 (TODO: 이 로직은 여기에 존재할게 아니라 애니메이션 클립에 있어야 함 (테스트용))

	// 애니메이션 시간 업데이트
	mElapsedTime += DeltaTime * 30.f * 160.f /** 0.2f*/;
	// mElapsedTime = 1.5f * 30.f * 160.f;

	// 애니메이션이 끝나면 처음으로
	if (mElapsedTime >= mSampleAnimation->GetEndTime() * 30.f * 160.f)
	{
		mElapsedTime = 0.f/*mSampleAnimation->GetStartTime() * 30.f * 160.f*/;
	}

	// 애니메이션에 따른 본 행렬 업데이트
	for (int32_t boneIndex = 0; boneIndex < mSkeletonData.Joints.size(); ++boneIndex)
	{
		const FJointData& joint = mSkeletonData.Joints[boneIndex];

		FMatrix parentAnimPose = joint.ParentIndex == -1
									 ? FMatrix::Identity
									 : mCurrentAnimationPose[mSkeletonData.Joints[joint.ParentIndex].Name];

		// 애니메이션 프레임간의 보간된 본 로컬 변환 행렬을 구함
		FMatrix thisFramePose = InterpolateBone(boneIndex, parentAnimPose, mElapsedTime);

		mCurrentAnimationPose[joint.Name] = thisFramePose;

		// x, y, z축 이 뒤집어 있으면 뒤집어 줌 (이미 로드시에 변환됐음)	TODO: 메서드 분리
		/*FVector v0, v1, v2, v3;
		v0 = thisFramePose.m[0];
		v1 = thisFramePose.m[1];
		v2 = thisFramePose.m[2];

		v3 = v1.Cross(v2);
		if (v0.Dot(v3) < 0.0f)
		{
			FMatrix matW;
			matW = XMMatrixScaling(-1.0f, -1.0f, -1.0f);
			thisFramePose *= matW;
		}*/
	}
}

void JSkeletalMeshObject::Draw()
{

	auto* deviceContext = IManager.RenderManager->GetImmediateDeviceContext();
	assert(deviceContext);

	// Topology 설정
	IManager.RenderManager->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 최종 본 행렬 업데이트
	UpdateBoneBuffer(deviceContext);
	deviceContext->VSSetShaderResources(5, 1, mInstanceBuffer_Bone.Resource_Bone.GetAddressOf());

	int32_t slots[2] = {0, 1};
	IManager.RenderManager->SetSamplerState(ESamplerState::LinearWrap, slots, 2);
	IManager.RenderManager->SetRasterState(IManager.GUIManager->IsRenderWireFrame()
											   ? ERasterState::WireFrame
											   : ERasterState::CullNone);
	IManager.RenderManager->SetBlendState(EBlendState::AlphaBlend);
	IManager.RenderManager->SetDepthStencilState(EDepthStencilState::DepthDefault);

	for (int32_t i = 0; i < mInstanceBuffer.size(); ++i)
	{
		auto& instanceBuffer = mInstanceBuffer[i];
		auto& meshData       = mPrimitiveMeshData[i];
		auto& subMeshes      = meshData->GetSubMesh();

		for (int32_t j = 0; j < instanceBuffer.Buffer_Vertex.size(); ++j)
		{
			auto& currMesh = subMeshes.empty() ? meshData : subMeshes[j];

			uint32_t offset = 0;

			mVertexSize = currMesh->GetVertexData()->GetVertexSize();

			currMesh->PassMaterial(deviceContext);
			int32_t indexNum = currMesh->GetVertexData()->IndexArray.size();

			// 버퍼 설정
			deviceContext->IASetVertexBuffers(0, 1, instanceBuffer.Buffer_Vertex[j].GetAddressOf(), &mVertexSize, &offset);
			deviceContext->IASetIndexBuffer(instanceBuffer.Buffer_Index[j].Get(), DXGI_FORMAT_R32_UINT, 0);

			deviceContext->DrawIndexed(indexNum, 0, 0);
		}
	}
	// JMeshObject::Draw();

}

void JSkeletalMeshObject::DrawID(uint32_t ID)
{
	JMeshObject::DrawID(ID);
}

void JSkeletalMeshObject::DrawBone()
{

	auto* deviceContext = Renderer.GetImmediateDeviceContext();
	assert(deviceContext);

	for (int32_t i = 0; i < mInstanceBuffer.size(); ++i)
	{
		Ptr<JSkeletalMesh> meshData = std::dynamic_pointer_cast<JSkeletalMesh>(mPrimitiveMeshData[i]);

		const auto& pose = meshData->GetSkinData()->GetBindPoseWorldMap();

		for (auto& mat : pose)
		{
			Ptr<JMeshObject> sphere = IManager.MeshManager->CreateOrClone("Game/Mesh/Sphere.jasset");
			if (sphere)
			{
				auto boneWorld = mat.second;
				auto parentInv = mSkinData->GetInfluenceBoneInverseBindPose(mat.first);

				FMatrix newMat = parentInv * mCurrentAnimationPose[mat.first] * boneWorld;
				sphere->UpdateBuffer(newMat);
				sphere->Draw();
			}
		}
		//
	}
}

FMatrix JSkeletalMeshObject::InterpolateBone(int32_t InIndex, const FMatrix& InParentMatrix, float InElapsedTime)
{
	FMatrix rotationMatrix = FMatrix::Identity;
	FMatrix scaleMatrix    = FMatrix::Identity;
	FMatrix invScaleMatrix = FMatrix::Identity;

	FQuaternion rotationQuat = FQuaternion::CreateFromRotationMatrix(rotationMatrix);
	FQuaternion scaleQuat    = FQuaternion::CreateFromRotationMatrix(scaleMatrix);

	float startTick = mSampleAnimation->GetStartTime() * 30.f * 160.f;
	float endTick   = 0.f;

	const auto track = mSampleAnimation->GetTracks()[InIndex];

	if (!track)
	{
		LOG_CORE_ERROR("Animation Track is empty");
		return FMatrix::Identity;
	}

	JAnimKey<FVector> StartTrack_Position;
	JAnimKey<FVector> EndTrack_Position;

	JAnimKey<FVector4> StartTrack_Rotation;
	JAnimKey<FVector4> EndTrack_Rotation;

	JAnimKey<FVector> StartTrack_Scale;
	JAnimKey<FVector> EndTrack_Scale;

	bool bShouldInterpolate = false;
	bool bEndTrack          = false;

	// Position
	FVector translation = FVector::ZeroVector;
	for (int32_t i = 0; i < track->TransformKeys.PositionKeys.size(); ++i)
	{
		if (track->TransformKeys.PositionKeys[i].Time * 30.f * 160.f > InElapsedTime)
		{
			EndTrack_Position = track->TransformKeys.PositionKeys[i];
			bEndTrack         = true;
			break;
		}
		StartTrack_Position = track->TransformKeys.PositionKeys[i];
		bShouldInterpolate  = true;
	}

	if (bShouldInterpolate)
	{
		translation = StartTrack_Position.Value;
		startTick   = StartTrack_Position.Time * 30.f * 160.f;
	}
	if (bEndTrack)
	{
		endTick     = EndTrack_Position.Time * 30.f * 160.f;
		translation = XMVectorLerp(XMLoadFloat3(&translation),
								   XMLoadFloat3(&EndTrack_Position.Value),
								   (InElapsedTime - startTick) / (endTick - startTick));
	}

	FMatrix translationMatrix = XMMatrixTranslation(translation.x, translation.y, translation.z);

	bShouldInterpolate = false;
	bEndTrack          = false;


	// Rotation
	for (int32_t i = 0; i < track->TransformKeys.RotationKeys.size(); ++i)
	{
		if (track->TransformKeys.RotationKeys[i].Time * 30.f * 160.f > InElapsedTime)
		{
			EndTrack_Rotation = track->TransformKeys.RotationKeys[i];
			bEndTrack         = true;
			break;
		}
		StartTrack_Rotation = track->TransformKeys.RotationKeys[i];
		bShouldInterpolate  = true;
	}

	if (bShouldInterpolate)
	{
		rotationQuat = FQuaternion{StartTrack_Rotation.Value};
		startTick    = StartTrack_Rotation.Time * 30.f * 160.f;
	}

	if (bEndTrack)
	{
		endTick      = EndTrack_Rotation.Time * 30.f * 160.f;
		rotationQuat = FQuaternion::Slerp(rotationQuat,
										  FQuaternion{EndTrack_Rotation.Value},
										  (InElapsedTime - startTick) / (endTick - startTick));
	}
	FMatrix  rotation;
	XMVECTOR quatv = XMLoadFloat4(&rotationQuat);
	XMStoreFloat4x4(&rotation, XMMatrixRotationQuaternion(quatv));

	// Scale
	bShouldInterpolate = false;
	bEndTrack          = false;

	FVector scale = FVector::ZeroVector;

	for (int32_t i = 0; i < track->TransformKeys.ScaleKeys.size(); ++i)
	{
		if (track->TransformKeys.ScaleKeys[i].Time * 30.f * 160.f > InElapsedTime)
		{
			EndTrack_Scale = track->TransformKeys.ScaleKeys[i];
			bEndTrack      = true;
			break;
		}
		StartTrack_Scale   = track->TransformKeys.ScaleKeys[i];
		bShouldInterpolate = true;
	}

	if (bShouldInterpolate)
	{
		scale     = StartTrack_Scale.Value;
		startTick = StartTrack_Scale.Time * 30.f * 160.f;
	}

	if (bEndTrack)
	{
		endTick = EndTrack_Scale.Time * 30.f * 160.f;
		scale   = XMVectorLerp(XMLoadFloat3(&scale),
							 XMLoadFloat3(&EndTrack_Scale.Value),
							 (InElapsedTime - startTick) / (endTick - startTick));
	}

	scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);


	FMatrix mat = scaleMatrix * rotation * translationMatrix * InParentMatrix;

	return mat;
}

bool JSkeletalMeshObject::GetAnimationTrack(float                InTime, const JArray<Ptr<JAnimBoneTrack>>& InTracks,
											Ptr<JAnimBoneTrack>& StartTrack, Ptr<JAnimBoneTrack>&           EndTrack)
{
	// for (int32_t i = 0; i < InTracks.size(); ++i)
	// {
	// 	if (InTracks[i]->IsTrackEmpty())
	// 	{
	// 		continue;
	// 	}
	//
	// 	if (InTracks[i]->TransformKeys.PositionKeys.Keys[0].Time <= InTime &&
	// 		InTracks[i]->TransformKeys.PositionKeys.Keys[InTracks[i]->TransformKeys.PositionKeys.Keys.size() - 1].Time >=
	// 		InTime)
	// 	{
	// 		StartTrack = InTracks[i];
	// 		EndTrack   = InTracks[i + 1];
	// 		return true;
	// 	}
	// }

	return false;
}
