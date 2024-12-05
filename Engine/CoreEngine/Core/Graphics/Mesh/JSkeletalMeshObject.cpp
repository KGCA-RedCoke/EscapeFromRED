#include "JSkeletalMeshObject.h"

#include "MMeshManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/JWorld.h"

JSkeletalMeshObject::JSkeletalMeshObject(const JText& InName, const JArray<Ptr<JMeshData>>& InData)
{
	
	mName       = InName;
	mVertexSize = sizeof(Vertex::FVertexInfo_Base);
	// mMeshConstantBuffer.MeshFlags = EnumAsByte(EMeshType::Skeletal);

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
		// JSkeletalMeshObject::CreateBuffers(TODO, TODO);
	}

	assert(mPrimitiveMeshData.size() > 0);

	mSkeletalMesh = std::dynamic_pointer_cast<JSkeletalMesh>(mPrimitiveMeshData[0]);

	mSampleAnimation = MakePtr<JAnimationClip>();
	if (!Utils::Serialization::DeSerialize("Game/Animation/Anim_Hands__Lantern_01_Walk.jasset", mSampleAnimation.get()))
	{
		LOG_CORE_ERROR("Failed to load animation object(Invalid Path maybe...): {0}",
					   "Game/Animation/Anim_Hands_Empty_Walk.jasset");
	}
	mSampleAnimation->SetSkeletalMesh(mSkeletalMesh);
	mSampleAnimation->Play();
}

JSkeletalMeshObject::JSkeletalMeshObject(const JWText& InName, const std::vector<Ptr<JMeshData>>& InData)
	: JMeshObject(InName, InData) {}

JSkeletalMeshObject::JSkeletalMeshObject(const JSkeletalMeshObject& Other)
	: JMeshObject(Other)
{
	mSkeletalMesh        = Other.mSkeletalMesh;
	mSampleAnimation     = Other.mSampleAnimation;
	mInstanceBuffer_Bone = Other.mInstanceBuffer_Bone;

	for (int32_t i = 0; i < Other.mMaterialInstances.size(); ++i)
	{
		mInstanceData[i].MaterialData.Flag |= (1 << 11);
	}
}

UPtr<IManagedInterface> JSkeletalMeshObject::Clone() const
{
	return MakeUPtr<JSkeletalMeshObject>(*this);
}

void JSkeletalMeshObject::UpdateBoneBuffer(ID3D11DeviceContext* InDeviceContext)
{
	// const auto& skinData  = mSkeletalMesh->GetSkinData();
	// const auto& animPoses = mSampleAnimation->GetAnimPoses();
	//
	// const int32_t boneCount = skinData->GetInfluenceBoneCount();
	// FMatrix       updateBoneMatrixBuffer[255];
	//
	// // 본 업데이트
	// for (int32_t i = 0; i < boneCount; ++i)
	// {
	// 	const JText jointName = skinData->GetInfluenceBoneName(i);
	// 	FMatrix     newPose   = skinData->GetInfluenceBoneInverseBindPose(jointName);
	//
	// 	// 최종 본 변환 행렬 = 본 로컬 변환 행렬 * 본 역바인드 행렬(부모 본의 역바인드 행렬)
	// 	updateBoneMatrixBuffer[i] = newPose * animPoses[i]; /* * newAnimMatrix (여기에 애니메이션 프레임간의 보간된 본 로컬 변환 행렬을 곱해줌) */
	// }
	//
	// Utils::DX::UpdateDynamicBuffer(
	// 							   InDeviceContext,
	// 							   mInstanceBuffer_Bone.Buffer_Bone.Get(),
	// 							   updateBoneMatrixBuffer,
	// 							   boneCount * sizeof(FMatrix));
	//
	// InDeviceContext->VSSetShaderResources(5, 1, mInstanceBuffer_Bone.Resource_Bone.GetAddressOf());

	// if (!mLightMesh)
	// {
	// 	mLightMesh         = GetWorld.MeshManager->CreateOrClone("Game/Mesh/SM_Flashlight_01.jasset");
	// 	mSocketOffsetRot.x = XMConvertToRadians(mSocketOffsetRot.x);
	// 	mSocketOffsetRot.y = XMConvertToRadians(mSocketOffsetRot.y);
	// 	mSocketOffsetRot.z = XMConvertToRadians(mSocketOffsetRot.z);
	// }
	//
	// // 1. 소켓 오프셋 행렬 생성 (로컬 변환)
	// auto locMat = DirectX::XMMatrixTranslation(mSocketOffsetLoc.x, mSocketOffsetLoc.y, mSocketOffsetLoc.z);
	//
	// auto    rotMat             = XMMatrixRotationRollPitchYawFromVector(mSocketOffsetRot);
	// FMatrix socketOffsetMatrix = FMatrix::Identity;
	// socketOffsetMatrix         = socketOffsetMatrix * rotMat * locMat;
	//
	// // 2. 본의 월드 변환 계산 (월드 바인드 포즈 * 본 애니메이션 변환)
	// FMatrix boneWorldMatrix = skinData->GetInfluenceWorldBindPose("hand_r") * socketOffsetMatrix * updateBoneMatrixBuffer[
	// 	mHand_r_Index];
	//
	// // 3. 소켓 변환 적용
	// mSocketTransform = boneWorldMatrix /** socketOffsetMatrix*/;
	//
	//
	// // 3. 모델 월드 변환 적용
	// mSocketTransform = socketOffsetMatrix *  skinData->GetInfluenceWorldBindPose("hand_r") *
	// 		updateBoneMatrixBuffer[mHand_r_Index] * mInstanceData[0].WorldMatrix;


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

	mMaterialInstances.resize(mPrimitiveMeshData[0]->GetSubMaterialNum());
	mInstanceData.resize(mPrimitiveMeshData[0]->GetSubMaterialNum());
	for (int32_t i = 0; i < mPrimitiveMeshData[0]->GetSubMaterialNum(); ++i)
	{
		JText materialPath;
		Utils::Serialization::DeSerialize_Text(materialPath, InFileStream);
		auto* matInstance = GetWorld.MaterialInstanceManager->Load(materialPath);

		SetMaterialInstance(matInstance, i);
		mInstanceData[i].MaterialData.Flag |= 1 << 11;
	}

	return true;
}

void JSkeletalMeshObject::Tick(float DeltaTime)
{
	mSampleAnimation->TickAnim(DeltaTime);
}

void JSkeletalMeshObject::AddInstance(float InCameraDistance)
{
	UpdateBoneBuffer(GetWorld.D3D11API->GetImmediateDeviceContext());

	// mLightMesh->UpdateInstance_Transform(mSocketTransform);
	// mLightMesh->AddInstance();

	auto&         meshData     = mPrimitiveMeshData[0];
	auto&         subMeshes    = meshData->GetSubMesh();
	const int32_t subMeshCount = subMeshes.empty() ? 1 : subMeshes.size();

	for (int32_t j = 0; j < subMeshCount; ++j)
	{
		auto& currMesh = subMeshes.empty() ? meshData : subMeshes[j];

		GetWorld.MeshManager->PushCommand(currMesh->GetHash(), mMaterialInstances[j], mInstanceData[j]);
	}

}

void JSkeletalMeshObject::Draw()
{
	auto* deviceContext = GetWorld.D3D11API->GetImmediateDeviceContext();
	assert(deviceContext);

	// 최종 본 행렬 업데이트
	UpdateBoneBuffer(deviceContext);

	JMeshObject::Draw();
}

void JSkeletalMeshObject::DrawID(uint32_t ID)
{
	JMeshObject::DrawID(ID);
}

void JSkeletalMeshObject::DrawBone()
{
	//
	// auto* deviceContext = G_Device.GetImmediateDeviceContext();
	// assert(deviceContext);
	//
	// for (int32_t i = 0; i < mGeometryBuffer.size(); ++i)
	// {
	// 	Ptr<JSkeletalMesh> meshData = std::dynamic_pointer_cast<JSkeletalMesh>(mPrimitiveMeshData[i]);
	//
	// 	const auto& pose = meshData->GetSkinData()->GetBindPoseWorldMap();
	//
	// 	for (auto& mat : pose)
	// 	{
	// 		Ptr<JMeshObject* sphere = GetWorld.MeshManager->CreateOrClone("Game/Mesh/Sphere.jasset");
	// 		if (sphere)
	// 		{
	// 			auto boneWorld = mat.second;
	// 			auto parentInv = mSkeletalMesh->GetSkinData()->GetInfluenceBoneInverseBindPose(mat.first);
	//
	// 			FMatrix newMat = parentInv * boneWorld;
	// 			sphere->UpdateInstance_Transform(newMat);
	// 			sphere->Draw();
	// 		}
	// 	}
	// 	//
	// }

}

void JSkeletalMeshObject::SetAnimation(JAnimationClip* InAnimation)
{
	if (mSampleAnimation)
	{
		mSampleAnimation->Stop();
		mSampleAnimation = nullptr;
	}

	// mSampleAnimation = InAnimation;
	mSampleAnimation->SetSkeletalMesh(mSkeletalMesh);
	mSampleAnimation->Play();
}
