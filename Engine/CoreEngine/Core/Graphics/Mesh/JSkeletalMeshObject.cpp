#include "JSkeletalMeshObject.h"

#include "MMeshManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Interface/JWorld.h"

JSkeletalMeshObject::JSkeletalMeshObject(const JText& InName, const JArray<Ptr<JMeshData>>& InData)
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
		// JSkeletalMeshObject::CreateBuffers(TODO, TODO);
	}

	assert(mPrimitiveMeshData.size() > 0);

	mSkeletalMesh = std::dynamic_pointer_cast<JSkeletalMesh>(mPrimitiveMeshData[0]);

	mSampleAnimation = MakePtr<JAnimationClip>();
	if (!Utils::Serialization::DeSerialize("Game/Animation/Lucy_Idle.jasset", mSampleAnimation.get()))
	{
		LOG_CORE_ERROR("Failed to load animation object(Invalid Path maybe...): {0}", "Game/Animation/Unreal Take.jasset");
	}
	mSampleAnimation->SetSkeletalMesh(mSkeletalMesh);
	mSampleAnimation->Play();
}

JSkeletalMeshObject::JSkeletalMeshObject(const JWText& InName, const std::vector<Ptr<JMeshData>>& InData)
	: JMeshObject(InName, InData) {}

JSkeletalMeshObject::JSkeletalMeshObject(const JMeshObject& Other)
	: JMeshObject(Other) {}

UPtr<IManagedInterface> JSkeletalMeshObject::Clone() const
{
	return MakeUPtr<JSkeletalMeshObject>(*this);
}

void JSkeletalMeshObject::CreateBuffers(ID3D11Device* InDevice, JHash<uint32_t, Buffer::FBufferGeometry>& InBufferList)
{
	ID3D11Device* device = GetWorld.D3D11API->GetDevice();
	assert(device);

	// JMeshObject::CreateBuffers(TODO, TODO);

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
	const auto& skinData  = mSkeletalMesh->GetSkinData();
	const auto& animPoses = mSampleAnimation->GetAnimPoses();

	const int32_t boneCount = skinData->GetInfluenceBoneCount();
	FMatrix       updateBoneMatrixBuffer[255];

	// 본 업데이트
	for (int32_t i = 0; i < boneCount; ++i)
	{
		const JText jointName = skinData->GetInfluenceBoneName(i);
		FMatrix     newPose   = skinData->GetInfluenceBoneInverseBindPose(jointName);

		// 최종 본 변환 행렬 = 본 로컬 변환 행렬 * 본 역바인드 행렬(부모 본의 역바인드 행렬)
		updateBoneMatrixBuffer[i] = newPose * animPoses[i]; /* * newAnimMatrix (여기에 애니메이션 프레임간의 보간된 본 로컬 변환 행렬을 곱해줌) */
	}

	Utils::DX::UpdateDynamicBuffer(
								   InDeviceContext,
								   mInstanceBuffer_Bone.Buffer_Bone.Get(),
								   updateBoneMatrixBuffer,
								   boneCount * sizeof(FMatrix));

	InDeviceContext->VSSetShaderResources(5, 1, mInstanceBuffer_Bone.Resource_Bone.GetAddressOf());
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

	for (int32_t i = 0; i < mPrimitiveMeshData[0]->GetSubMaterialNum(); ++i)
	{
		JText materialPath;
		Utils::Serialization::DeSerialize_Text(materialPath, InFileStream);
		auto* matInstance = GetWorld.MaterialInstanceManager->CreateOrLoad(materialPath);
		mMaterialInstances.push_back(matInstance);
	}

	return true;
}

void JSkeletalMeshObject::Tick(float DeltaTime)
{
	mSampleAnimation->TickAnim(DeltaTime);
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
	// 			sphere->UpdateBuffer(newMat);
	// 			sphere->Draw();
	// 		}
	// 	}
	// 	//
	// }

}

void JSkeletalMeshObject::SetAnimation(const Ptr<JAnimationClip>& InAnimation)
{
	if (mSampleAnimation)
	{
		mSampleAnimation->Stop();
		mSampleAnimation = nullptr;
	}

	mSampleAnimation = InAnimation;
	mSampleAnimation->SetSkeletalMesh(mSkeletalMesh);
	mSampleAnimation->Play();
}
