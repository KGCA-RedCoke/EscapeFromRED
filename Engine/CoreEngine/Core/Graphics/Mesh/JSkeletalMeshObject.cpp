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

	CreateBuffers();

	return true;
}

void JSkeletalMeshObject::Tick(float DeltaTime)
{
	mSampleAnimation->TickAnim(DeltaTime);
}

void JSkeletalMeshObject::Draw()
{
	auto* deviceContext = IManager.RenderManager->GetImmediateDeviceContext();
	assert(deviceContext);

	// Topology 설정
	IManager.RenderManager->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 최종 본 행렬 업데이트
	UpdateBoneBuffer(deviceContext);

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
				auto parentInv = mSkeletalMesh->GetSkinData()->GetInfluenceBoneInverseBindPose(mat.first);

				FMatrix newMat = parentInv * boneWorld;
				sphere->UpdateBuffer(newMat);
				sphere->Draw();
			}
		}
		//
	}
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
