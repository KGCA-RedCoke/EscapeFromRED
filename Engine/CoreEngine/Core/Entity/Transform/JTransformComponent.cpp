#include "common_pch.h"
// #include "JTransformComponent.h"
//
// #include "Core/Graphics/Shader/JDXObject.h"
// #include "Core/Interface/MManagerInterface.h"
// #include "Core/Utils/ObjectLoader/FbxObject.h"
//
//
// JTransformComponent::JTransformComponent()
// {
// 	// mDXObject = IManager.ShaderManager.CreateOrLoad(L"Shader/Basic.hlsl");
// }
//
// void JTransformComponent::PreRender()
// {
// 	mDXObject->PreRender();
// }
//
// void JTransformComponent::Render()
// {
// 	PreRender();
//
// 	mDXObject->Render();
//
// 	auto* context = G_Context.GetImmediateDeviceContext();
// 	assert(context);
//
// 	{
// 		int32_t drawObjectNum = mDataList.size();
//
// 		for (int32_t i = 0; i < drawObjectNum; ++i)
// 		{
// 			auto* mesh = mMeshList[i].get();
//
// 			if (mesh->ClassType == EMeshType::BIPED || mesh->ClassType == EMeshType::BONE)
// 				continue;
//
// 			UpdateConstantBuffer();
//
// 			{
// 				FMatrix*                 matrix;
// 				D3D11_MAPPED_SUBRESOURCE mappedFaceDest;
//
// 				CheckResult(context->Map(mBoneBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedFaceDest));
//
// 				matrix = static_cast<FMatrix*>(mappedFaceDest.pData);
// 				for (int32_t j = 0; j < mesh->InfluenceNames.size(); ++j)
// 				{
// 					matrix[j] = mesh->BindPoseMap[mesh->InfluenceNames[j]] * mesh->MatrixList[j];
// 				}
//
// 				context->Unmap(mBoneBuffer.Get(), 0);
// 			}
//
// 			context->VSSetShaderResources(1, 1, mBoneBufferSRV.GetAddressOf());
//
// 			if (!mesh->SubMesh.empty())
// 			{}
// 			else
// 			{
// 				if (mesh->FaceNum < 1)
// 					continue;
// 				// context->PSSetShaderResources(0, 1, );
// 			}
// 		}
// 	}
//
// 	PostRender();
// }
//
// void JTransformComponent::PostRender()
// {
// 	UpdateConstantBuffer();
// 	mDXObject->PostRender();
// }
//
// void JTransformComponent::UpdateConstantBuffer()
// {
// 	if (mParentTransformComp && mParentTransformComp->mDXObject->GetCBuffer())
// 	{
// 		G_Context.GetImmediateDeviceContext()->UpdateSubresource(mParentTransformComp->mDXObject->GetCBuffer(),
// 																 0,
// 																 nullptr,
// 																 &mParentTransformComp->mConstantBufferData,
// 																 0,
// 																 0);
// 	}
// 	else if (mDXObject->GetCBuffer())
// 	{
// 		G_Context.GetImmediateDeviceContext()->UpdateSubresource(mDXObject->GetCBuffer(),
// 																 0,
// 																 nullptr,
// 																 &mConstantBufferData,
// 																 0,
// 																 0);
// 	}
// }
//
// void JTransformComponent::SetMesh(FbxFile* InFbxObj)
// {
// 	// Create a bone matrix buffer
// 	// It will be updated more than once per frame (in a typical game) so make it dynamic
// 	D3D11_BUFFER_DESC vbdesc =
// 	{
// 		255 * sizeof(FMatrix),
// 		D3D11_USAGE_DYNAMIC,
// 		D3D11_BIND_SHADER_RESOURCE,
// 		D3D11_CPU_ACCESS_WRITE,
// 		0
// 	};
// 	CheckResult(G_Context.GetDevice()->CreateBuffer(&vbdesc, nullptr, mBoneBuffer.GetAddressOf()));
//
// 	// Again, we need a resource view to use it in the shader
// 	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
// 	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
// 	SRVDesc.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
// 	SRVDesc.ViewDimension        = D3D11_SRV_DIMENSION_BUFFER;
// 	SRVDesc.Buffer.ElementOffset = 0;
// 	SRVDesc.Buffer.ElementWidth  = 255 * 4;
// 	CheckResult(G_Context.GetDevice()->
// 						  CreateShaderResourceView(mBoneBuffer.Get(), &SRVDesc, mBoneBufferSRV.GetAddressOf()));
//
// 	// mMeshList = InFbxObj->mMeshList;
// 	// mDataList = InFbxObj->mDataList;
//
// 	uint32_t vertexBufOffset = 0;
// 	uint32_t indexBufOffset  = 0;
//
// 	for (int32_t i = 0; i < mDataList.size(); ++i)
// 	{
// 		auto data = mDataList[i].get();
// 		auto mesh = mMeshList[i].get();
//
// 		// subMesh 존재
// 		if (!mesh->SubMesh.empty())
// 		{
// 			/*for (int32_t subMeshIdx = 0; subMeshIdx < mesh->SubMesh.size(); ++subMeshIdx)
// 			{
// 				auto subData = data->SubMesh[subMeshIdx].get();
// 				auto subMesh = mesh->SubMesh[subMeshIdx].get();
//
// 				if (subData->VertexArray.size() < 3)
// 					continue;
//
// 				subMesh->m_dxobj.m_iNumVertex  = subData->VertexArray.size();
// 				subMesh->m_dxobj.m_iVertexSize = mVertexSize;
//
// 				g_pImmediateContext->UpdateSubresource(
// 													   pVB,
// 													   0,
// 													   &ptSubMesh->m_dxobj.m_BoxVB,
// 													   (uint8_t*)&pSubData->m_VertexArray.at(0),
// 													   0,
// 													   0);
//
//
// 				/*g_pImmediateContext->CopySubresourceRegion(
// 				m_dxobj.g_pVertexBuffer.Get(), 0, iBeginPos, 0, 0,
// 				(void*)&pSubMesh->m_VertexArray.at(0),
// 				0, &pSubMesh->m_dxobj.m_BoxVB);#1#
//
// 				subMesh->m_dxobj.m_iBeginVB = vbOffset;
// 				vbOffset += subMesh->m_dxobj.m_iNumVertex;
// 				dstOffset = subMesh->m_dxobj.m_BoxVB.right;
//
// 				subMesh->m_dxobj.m_iNumIndex = subData->IndexArray.size();
//
// 				g_pImmediateContext->UpdateSubresource(pIB,
// 													   0,
// 													   &ptSubMesh->m_dxobj.m_BoxIB,
// 													   (void*)&pSubData->m_IndexArray.at(0),
// 													   0,
// 													   0);
//
// 				subMesh->m_dxobj.m_iBeginIB = ibOffset;
// 				ibOffset += ptSubMesh->m_dxobj.m_iNumIndex;
// 				dstibOffset = ptSubMesh->m_dxobj.m_BoxIB.right;
//
// 				//texture
// 				if (ptSubMesh->m_iDiffuseTex <= 0)
// 					continue;
// 				subMesh->m_dxobj.g_pTextureSRV = I_Texture.GetPtr(ptSubMesh->m_iDiffuseTex)->m_pTextureSRV;
// 			}*/
// 		}
// 		// Mesh만 존재
// 		else
// 		{
// 			if (data->VertexArray.size() < 3)
// 				continue;
//
// 			uint32_t vertexNum = data->VertexArray.size();
// 			uint32_t indexNum  = data->IndexArray.size();
//
// 			mDXObject->UpdateVertexData((void*)&data->VertexArray.at(0),
// 										  vertexNum,
// 										  sizeof(Vertex::FVertexInfo_Base),
// 										  vertexBufOffset);
// 			vertexBufOffset += vertexNum;
//
//
// 			mDXObject->UpdateIndexData(&data->IndexArray.at(0), indexNum, indexBufOffset);
// 			indexBufOffset += indexNum;
//
// 			//texture
// 			if (mesh->DiffuseTex <= 0)
// 				continue;
// 			// mesh->m_dxobj.g_pTextureSRV = I_Texture.GetPtr(mesh->DiffuseTex)->m_pTextureSRV;
// 		}
// 	}
// }
