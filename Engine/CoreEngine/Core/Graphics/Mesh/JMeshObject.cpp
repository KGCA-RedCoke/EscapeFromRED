#include "JMeshObject.h"

#include <ranges>

#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Mesh/JMeshData.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Graphics/DXUtils.h"


JMeshObject::JMeshObject(const JText& InName, const JArray<Ptr<JMeshData>>& InData)
	: mName(InName),
	  mVertexSize(sizeof(Vertex::FVertexInfo_Base))
{
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
		CreateBuffers();
	}
}

JMeshObject::JMeshObject(const JWText& InName, const JArray<Ptr<JMeshData>>& InData)
	: JMeshObject(WString2String(InName), InData) {}

JMeshObject::JMeshObject(const JMeshObject& Other)
	: mName(Other.mName),
	  mInstanceBuffer(Other.mInstanceBuffer),
	  mVertexSize(Other.mVertexSize)
{
	JArray<Ptr<JMeshData>> clonedData;
	for (const auto& data : Other.mPrimitiveMeshData)
	{
		clonedData.push_back(std::dynamic_pointer_cast<JMeshData>(data->Clone()));
	}
	mPrimitiveMeshData = clonedData;
}

Ptr<IManagedInterface> JMeshObject::Clone() const
{
	auto cloned = MakePtr<JMeshObject>(*this);
	return cloned;
}

uint32_t JMeshObject::GetHash() const
{
	return StringHash(mName.c_str());
}

uint32_t JMeshObject::GetType() const
{
	return bIsSkeletalMesh ? HASH_ASSET_TYPE_SKELETAL_MESH : HASH_ASSET_TYPE_STATIC_MESH;
}

bool JMeshObject::Serialize_Implement(std::ofstream& FileStream)
{
	if (!Utils::Serialization::SerializeMetaData(FileStream, this))
	{
		return false;
	}
	// Mesh Name
	Utils::Serialization::Serialize_Text(mName, FileStream);

	// Primitive Mesh Data
	int32_t meshDataSize = mPrimitiveMeshData.size();
	Utils::Serialization::Serialize_Primitive(&meshDataSize, sizeof(meshDataSize), FileStream);

	for (int32_t i = 0; i < meshDataSize; ++i)
	{
		mPrimitiveMeshData[i]->Serialize_Implement(FileStream);
	}

	return true;
}

bool JMeshObject::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, HASH_ASSET_TYPE_STATIC_MESH))
	{
		// File pointer to header
		InFileStream.seekg(sizeof(JAssetHeader), std::ios::beg);
		if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, HASH_ASSET_TYPE_SKELETAL_MESH))
		{
			return false;
		}
	}

	// Mesh Name
	Utils::Serialization::DeSerialize_Text(mName, InFileStream);

	// Primitive Mesh Data
	int32_t meshDataSize;
	Utils::Serialization::DeSerialize_Primitive(&meshDataSize, sizeof(meshDataSize), InFileStream);

	for (int32_t i = 0; i < meshDataSize; ++i)
	{
		auto archivedData = MakePtr<JMeshData>();
		archivedData->DeSerialize_Implement(InFileStream);
		mPrimitiveMeshData.push_back(archivedData);
	}

	CreateBuffers();

	return true;
}

/** 버퍼는 메모리에 올릴 때 한번만 생성되고 공유 */
void JMeshObject::CreateBuffers()
{
	if (mInstanceBuffer.size() > 0)
	{
		return;
	}

	ID3D11Device* device = IManager.RenderManager->GetDevice();
	assert(device);

	const int32_t meshDataSize = mPrimitiveMeshData.size();

	mInstanceBuffer.resize(meshDataSize);
	mInstanceBuffer_Bone.resize(meshDataSize);

	for (int32_t i = 0; i < meshDataSize; ++i)
	{
		auto& mesh                = mPrimitiveMeshData[i];
		auto& instanceBuffer      = mInstanceBuffer[i];
		auto& instanceBuffer_Bone = mInstanceBuffer_Bone[i];
		auto& subMeshes           = mesh->GetSubMesh();

		instanceBuffer.Resize(subMeshes.empty() ? 1 : subMeshes.size());
		instanceBuffer_Bone.Resize(subMeshes.empty() ? 1 : subMeshes.size());

		if (mesh->GetClassType() == EMeshType::Skeletal)
		{
			bIsSkeletalMesh = true;
		}

		for (int32_t j = 0; j < instanceBuffer.Buffer_Vertex.size(); ++j)
		{
			auto& data = subMeshes.empty()
							 ? mesh->GetVertexData()
							 : subMeshes[j]->GetVertexData();

			// Vertex 버퍼 생성
			Utils::DX::CreateBuffer(device,
									D3D11_BIND_VERTEX_BUFFER,
									reinterpret_cast<void**>(&data->VertexArray.at(0)),
									mVertexSize,
									data->VertexArray.size(),
									instanceBuffer.Buffer_Vertex[j].GetAddressOf());

			// Index 버퍼 생성
			Utils::DX::CreateBuffer(device,
									D3D11_BIND_INDEX_BUFFER,
									reinterpret_cast<void**>(&data->IndexArray.at(0)),
									SIZE_INDEX_BUFFER,
									data->IndexArray.size(),
									instanceBuffer.Buffer_Index[j].GetAddressOf());

			if (mesh->GetClassType() == EMeshType::Skeletal)
			{
				// Bone 버퍼 생성
				Utils::DX::CreateBuffer(device,
										D3D11_BIND_SHADER_RESOURCE,
										nullptr,
										sizeof(FMatrix),
										SIZE_MAX_BONE_NUM,
										instanceBuffer_Bone.Buffer_Bone[j].GetAddressOf());

				D3D11_SHADER_RESOURCE_VIEW_DESC boneSRVDesc;
				ZeroMemory(&boneSRVDesc, sizeof(boneSRVDesc));
				{
					boneSRVDesc.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;	// 4x4 Matrix
					boneSRVDesc.ViewDimension        = D3D11_SRV_DIMENSION_BUFFER;		// Buffer
					boneSRVDesc.Buffer.ElementOffset = 0;
					boneSRVDesc.Buffer.ElementWidth  = SIZE_MAX_BONE_NUM * 4;
					device->CreateShaderResourceView(instanceBuffer_Bone.Buffer_Bone[j].Get(),
													 &boneSRVDesc,
													 instanceBuffer_Bone.Resource_Bone[j].GetAddressOf());
				}
			}

		}
	}
}

void JMeshObject::Update(float DeltaTime)
{}

void JMeshObject::UpdateBuffer(const FMatrix& InWorldMatrix)
{
	mWorldMatrix = InWorldMatrix;

	auto* deviceContext = IManager.RenderManager->GetImmediateDeviceContext();
	assert(deviceContext);

	for (int32_t i = 0; i < mInstanceBuffer.size(); ++i)
	{
		auto& instanceBuffer = mInstanceBuffer[i];
		if (bIsSkeletalMesh)
		{
			auto& boneBuffer = mInstanceBuffer_Bone[i];
		}

		auto& meshData  = mPrimitiveMeshData[i];
		auto& subMeshes = meshData->GetSubMesh();

		const bool bIsSkeletal = meshData->GetClassType() == EMeshType::Skeletal;

		for (int32_t j = 0; j < instanceBuffer.Buffer_Vertex.size(); ++j)
		{
			auto& currMesh = subMeshes.empty() ? meshData : subMeshes[j];
			currMesh->UpdateWorldMatrix(deviceContext, XMMatrixTranspose(mWorldMatrix));
		}
	}
}

void JMeshObject::Draw()
{
	auto* deviceContext = IManager.RenderManager->GetImmediateDeviceContext();
	assert(deviceContext);

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

			// Topology 설정
			IManager.RenderManager->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			currMesh->PassMaterial(deviceContext);
			int32_t indexNum = currMesh->GetVertexData()->IndexArray.size();

			// 버퍼 설정
			deviceContext->IASetVertexBuffers(0, 1, instanceBuffer.Buffer_Vertex[j].GetAddressOf(), &mVertexSize, &offset);
			deviceContext->IASetIndexBuffer(instanceBuffer.Buffer_Index[j].Get(), DXGI_FORMAT_R32_UINT, 0);

			int32_t slots[2] = {0, 1};
			IManager.RenderManager->SetSamplerState(ESamplerState::LinearWrap, slots, 2);
			IManager.RenderManager->SetRasterState(IManager.GUIManager->IsRenderWireFrame()
													   ? ERasterState::WireFrame
													   : ERasterState::CullNone);
			IManager.RenderManager->SetBlendState(EBlendState::AlphaBlend);
			IManager.RenderManager->SetDepthStencilState(EDepthStencilState::DepthDefault);

			deviceContext->DrawIndexed(indexNum, 0, 0);
		}
	}
}

void JMeshObject::DrawID(uint32_t ID)
{
	auto* deviceContext = IManager.RenderManager->GetImmediateDeviceContext();
	assert(deviceContext);

	for (int32_t i = 0; i < mInstanceBuffer.size(); ++i)
	{
		auto& instanceBuffer = mInstanceBuffer[i];
		auto& meshData       = mPrimitiveMeshData[i];
		auto& subMeshes      = meshData->GetSubMesh();

		for (int32_t j = 0; j < instanceBuffer.Buffer_Vertex.size(); ++j)
		{
			uint32_t offset = 0;
			int32_t  indexNum;

			// Topology 설정
			IManager.RenderManager->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// 버퍼 설정
			deviceContext->IASetVertexBuffers(0, 1, instanceBuffer.Buffer_Vertex[j].GetAddressOf(), &mVertexSize, &offset);
			deviceContext->IASetIndexBuffer(instanceBuffer.Buffer_Index[j].Get(), DXGI_FORMAT_R32_UINT, 0);

			auto idShader = IManager.ShaderManager->IDShader;

			auto cam = IManager.CameraManager->GetCurrentMainCam();

			CBuffer::Space space;
			space.Model      = XMMatrixTranspose(mWorldMatrix);
			space.View       = XMMatrixTranspose(cam->GetViewMatrix());
			space.Projection = XMMatrixTranspose(cam->GetProjMatrix());

			idShader->UpdateConstantData(deviceContext,
										 CBuffer::NAME_CONSTANT_BUFFER_SPACE,
										 &space);

			FVector4 color = Hash2Color(ID);
			idShader->UpdateConstantData(deviceContext, CBuffer::NAME_CONSTANT_BUFFER_COLOR_ID, &color);
			idShader->BindShaderPipeline(deviceContext);

			if (subMeshes.empty())
			{
				indexNum = meshData->GetVertexData()->IndexArray.size();
			}
			else
			{
				indexNum = subMeshes[j]->GetVertexData()->IndexArray.size();
			}

			IManager.RenderManager->SetBlendState(EBlendState::Opaque);

			deviceContext->DrawIndexed(indexNum, 0, 0);
		}
	}
}

void JMeshObject::DrawBone()
{
	// if (!bIsSkeletalMesh)
	// {
	// 	// LOG_CORE_ERROR("This mesh object is not skeletal mesh.");
	// 	return;
	// }
	// auto* deviceContext = IManager.RenderManager->GetImmediateDeviceContext();
	// assert(deviceContext);
	//
	// for (int32_t i = 0; i < mInstanceBuffer.size(); ++i)
	// {
	// 	auto& meshData = mPrimitiveMeshData[i];
	//
	// 	const auto& pose = meshData->GetBindPoseMap();
	//
	// 	Ptr<JMeshObject> sphere = IManager.MeshManager->CreateOrClone("Game/Mesh/Sphere.jasset");
	// 	if (sphere)
	// 	{
	// 		sphere->UpdateBuffer(mWorldMatrix  * pose.at("root"));
	// 		sphere->Draw();
	// 		
	// 		sphere->UpdateBuffer(mWorldMatrix  * pose.at("pelvis"));
	// 		sphere->Draw();
	// 		
	// 		sphere->UpdateBuffer(mWorldMatrix  * pose.at("spine_01"));
	// 		sphere->Draw();
	// 		
	// 		sphere->UpdateBuffer(mWorldMatrix  * pose.at("spine_02"));
	// 		sphere->Draw();
	// 		
	// 		sphere->UpdateBuffer(mWorldMatrix  * pose.at("spine_03"));
	// 		sphere->Draw();
	// 		
	// 		sphere->UpdateBuffer(mWorldMatrix  * pose.at("neck_01"));
	// 		sphere->Draw();
	// 		
	// 		sphere->UpdateBuffer(mWorldMatrix  * pose.at("head"));
	// 		sphere->Draw();
	//
	// 		sphere->UpdateBuffer(mWorldMatrix  * pose.at("thigh_l"));
	// 		sphere->Draw();
	//
	// 		sphere->UpdateBuffer(mWorldMatrix  * pose.at("thigh_twist_01_l"));
	// 		sphere->Draw();
	//
	//
	// 		sphere->UpdateBuffer(mWorldMatrix  * pose.at("calf_l"));
	// 		sphere->Draw();
	//
	// 		sphere->UpdateBuffer(mWorldMatrix  * pose.at("calf_twist_01_l"));
	// 		sphere->Draw();
	//
	// 		sphere->UpdateBuffer(mWorldMatrix  * pose.at("thigh_r"));
	// 		sphere->Draw();
	// 		
	// 		sphere->UpdateBuffer(mWorldMatrix  * pose.at("calf_r"));
	// 		sphere->Draw();
	// 		
	// 		sphere->UpdateBuffer(mWorldMatrix  * pose.at("calf_twist_01_r"));
	// 		sphere->Draw();
	// 	}
	// 	
	// 	
	// 	for (auto& mat : pose | std::views::values)
	// 	{
	// 		Ptr<JMeshObject> sphere = IManager.MeshManager->CreateOrClone("Game/Mesh/Sphere.jasset");
	// 		if (sphere)
	// 		{
	// 			sphere->UpdateBuffer(mWorldMatrix  * mat);
	// 			sphere->Draw();
	// 		}
	// 	}
	//
	// }
}
