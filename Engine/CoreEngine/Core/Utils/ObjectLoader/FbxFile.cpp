#include "FbxFile.h"
#include "FbxUtils.h"
#include "Core/Graphics/Mesh/JMesh.h"
#include "Core/Interface/MManagerInterface.h"

namespace Utils::Fbx
{
	FbxManager* g_fbx_manager = nullptr;

	FbxFile::FbxFile(const JText& InName)
		: mFileName(InName),
		  mFbxImporter(nullptr),
		  mFbxScene(nullptr) {}

	FbxFile::FbxFile(const JWText& InName)
		: mFileName(WString2String(InName)),
		  mFbxImporter(nullptr),
		  mFbxScene(nullptr) {}

	FbxFile::~FbxFile()
	{
		Release();
	}

	void FbxFile::Initialize()
	{
		if (!g_fbx_manager)
		{
			g_fbx_manager = FbxManager::Create();
		}
		assert(g_fbx_manager);

		mFbxImporter = FbxImporter::Create(g_fbx_manager, "");
		mFbxScene    = FbxScene::Create(g_fbx_manager, "");

		assert(mFbxImporter);
		assert(mFbxScene);

		bool bSuccess = mFbxImporter->Initialize(mFileName.c_str(), -1, g_fbx_manager->GetIOSettings());
		if (!bSuccess)
		{
			LOG_CORE_ERROR("Failed to initialize fbx importer, {0} {1}", __FILE__, __LINE__);
			return;
		}

		bSuccess = mFbxImporter->Import(mFbxScene);
		if (!bSuccess)
		{
			LOG_CORE_ERROR("Failed to import fbx importer, {0} {1}", __FILE__, __LINE__);
			return;
		}

		// front -> y | right -> x | up -> z (y, z 축이 바뀐 형태) 
		FbxAxisSystem::MayaZUp.ConvertScene(mFbxScene);
		FbxSystemUnit::cm.ConvertScene(mFbxScene);
	}

	void FbxFile::Release()
	{
		if (mFbxScene)
			mFbxScene->Destroy();
		if (mFbxImporter)
			mFbxImporter->Destroy();
		if (g_fbx_manager)
			g_fbx_manager->Destroy();
	}

	bool FbxFile::Load()
	{
		Initialize();

		ProcessLoad();

		Convert();

		return true;
	}

	bool FbxFile::Load(const char* InFilePath)
	{
		mFileName = InFilePath;

		Initialize();

		return Load();
	}

	bool FbxFile::Convert()
	{
		// TODO: Load() ->(then) Convert는 다른 스레드의 작업큐로 넣어야 함.
		for (int32_t meshIndex = 0; meshIndex < mMeshList.size(); ++meshIndex)
		{
			auto mesh = mMeshList[meshIndex].get();
			auto data = mesh->mVertexData.get();

			mesh->mIndex   = meshIndex;
			mesh->mFaceNum = data->FaceCount;

			// 부모 노드 존재시에 자식 노드로 등록
			if (mesh->mParentMesh)
			{
				mesh->mParentMesh->mChildMesh.push_back(mesh);
			}

			// 메시 타입이 GEOM(Static Mesh)일 경우만 우선 처리
			if (mesh->mClassType == EMeshType::GEOM)
			{
				const int32_t materialNum = mesh->mMaterialRefNum - 1;
				assert(materialNum >= 0);

				std::vector<Ptr<JMaterial>> materials = mMaterialList[materialNum];
				if (mesh->mFaceNum > 0 && !materials.empty())
				{
					// 머티리얼이 한개일 경우 서브메시를 사용하지 않는다.
					if (materials.size() == 1)
					{
						const Ptr<JMaterial> subMaterial = materials[0];

						// 단일 메시를 생성한다 (실제 생성이 아니라 정점, 인덱스 배열을 생성).
						data->GenerateIndexArray(data->TriangleList);

						if (subMaterial->HasMaterial())
						{
							mesh->mMaterial = subMaterial;
						}
						else
						{
							mesh->mMaterial = Utils::Material::s_DefaultMaterial;
						}

						mNumVertex += data->VertexArray.size();
						mNumIndex += data->IndexArray.size();
					}
					else
					{
						int iAddCount = 0;
						// for (int iSub = 0; iSub < materials.size(); iSub++)
						// {
						// 	auto pSubMesh = mesh->SubMesh[iSub].get();
						// 	auto pSubData = data->SubMesh[iSub].get();
						//
						// 	// 2번 인자값=-1 이면 Face Count(_countof )를 계산하지 않는다.
						// 	pSubData->GenerateIndexArray(pSubData->TriangleList, -1, 0);
						// 	pSubMesh->DiffuseTex = -1;
						//
						// 	CFbxMaterial* pSubMtrl = materials[iSub];
						// 	if (pSubMtrl->mParams.size() > 0)
						// 	{
						// 		JWText name;
						// 		name = String2WString(pSubMtrl->mParams[0].StringValue);
						// 		// pSubMesh->DiffuseTex = I_Texture.Add(
						// 		// 									 g_pd3dDevice,
						// 		// 									 name.c_str(),
						// 		// 									 m_szDirName.c_str());
						// 	}
						// 	mNumVertex += pSubData->VertexArray.size();
						// 	mNumIndex += pSubData->IndexArray.size();
						//
						// 	pSubData->FaceCount = pSubData->IndexArray.size() / 3;
						// 	pSubMesh->FaceNum   = pSubData->IndexArray.size() / 3;
						// }
					}
				}
			}
		}
		return true;
	}

	void FbxFile::ProcessLoad()
	{
		FbxNode* root = mFbxScene->GetRootNode();
		assert(root, "empty scene(node x)");

		// Parse Skeleton 
		ParseNode_Recursive(root, FbxNodeAttribute::EType::eSkeleton);
		// Parse Mesh
		ParseNode_Recursive(root, FbxNodeAttribute::EType::eMesh);

		// TODO: ParseAnimation();
	}

	void FbxFile::ParseNode_Recursive(FbxNode*          InNode, FbxNodeAttribute::EType NodeAttribute,
									  const Ptr<JMesh>& InParentMeshData)
	{
		if (!InNode)
			return;

		/// 모델링 툴에서 보면 알겠지만 노드는 메시, 라이트, 카메라, 스켈레톤 등 엄청나게 많은 타입이 있다.
		/// 이 노드 어떤 타입인지는 GetAttributeType()로 확인할 수 있고
		/// 개체를 얻으려면 GetAttribute로 구하여 reinterpret_cast로 캐스팅하면 된다.
		const FbxNodeAttribute* attribute = InNode->GetNodeAttribute();

		/// 현재 노드가 파싱하려는 노드 타입과 일치하면 파싱을 시작한다.
		/// 즉, 파싱된 정보를 담을 MeshData를 생성
		const Ptr<JMesh> meshData = MakePtr<JMesh>();
		if (attribute && attribute->GetAttributeType() == NodeAttribute)
		{
			{
				meshData->mName           = InNode->GetName();
				meshData->mParentMesh     = InParentMeshData;
				meshData->mMaterialRefNum = 0;
				meshData->mFaceNum        = 0;
				meshData->mVertexData     = MakePtr<JData<Vertex::FVertexInfo_Base>>();;
			}
			mMeshList.push_back(meshData);

			switch (NodeAttribute)
			{
			case FbxNodeAttribute::eSkeleton:
				meshData->mClassType = EMeshType::BONE;
			// TODO: ParseSkeleton();
				break;
			case FbxNodeAttribute::eMesh:
				meshData->mClassType = EMeshType::GEOM;
				ParseMesh(InNode, meshData);
				break;
			default:
				break;
			}
		}

		// 자식 노드를 재귀적으로 파싱
		for (int32_t i = 0; i < InNode->GetChildCount(); ++i)
		{
			ParseNode_Recursive(InNode->GetChild(i), NodeAttribute, meshData);
		}
	}

	void FbxFile::ParseMesh(FbxNode* InNode, Ptr<JMesh> InMeshData)
	{
		if (!InNode || !InNode->GetMesh())
			return;

		// fbx에서 정의된 메시를 가져오자.
		FbxMesh* fbxMesh = InNode->GetMesh();

		// 레이어 정보(Geometry (UV, Tangents, Normal, Material, Color...))부터 해석하자.
		FLayer layer = ParseMeshLayer(InNode->GetMesh(), InMeshData);

		const int32_t     polygonCount = fbxMesh->GetPolygonCount(); //  삼각 or 사각 poly 
		const int32_t     vertexCount  = fbxMesh->GetControlPointsCount(); // 정점 개수 반환
		const FbxVector4* vertices     = fbxMesh->GetControlPoints(); // 모든 정점 좌표 반환(메시의 로컬 좌표계)

		int32_t polygonSize; // triangle -> 3 | square -> 4
		int32_t polygonFaceCount; // triangle -> 1| square -> 2 (triangle * 2)
		int32_t curPolyIndex = 0;

		/// 삼각형 폴리곤(또는 사각형이겠지만... 거의 삼각형임)을 순회하면서 정점을 파싱
		/// 삼각형으로 강제하고싶으면 FbxConverter에서 삼각형으로 변환하도록 설정하자.
		// NOTE: FbxConverter::Triangulate(scene, true) -> 삼각형을 새로 만들기 때문에 추가적인 처리가 필요하다는데 찾아봐야한다.
		for (int32_t polygonIdx = 0; polygonIdx < polygonCount; ++polygonIdx)
		{
			polygonSize      = fbxMesh->GetPolygonSize(polygonIdx); // 삼각형이면 3, 사각형이면 4
			polygonFaceCount = polygonSize - 2;						// 대부분 1, 2일 것

			// (대부분 삼각형이므로 polygonSize == 3 && polygonFaceCount == 1임)
			assert(polygonSize >= 3);		// 삼각형 이상의 폴리곤이여야 한다.
			assert(polygonFaceCount >= 1);	// 삼각형이 하나 이상이여야 한다.

			// 메시데이터를 로드하는데 머티리얼이 없다는건 뭔가 잘못됐다.
			int32_t materialIndex = 0;
			assert(!layer.VertexMaterialSets.empty(), "Material is empty");

			// 머티리얼을 정점에 매핑하는 방법
			// 지금은 Layer0의 머티리얼만 사용 
			switch (layer.VertexMaterialSets[0]->GetMappingMode())
			{
			case FbxLayerElement::eByPolygon:
				switch (layer.VertexMaterialSets[0]->GetReferenceMode())
				{
				// 머티리얼이 직접 매핑되어있는 경우
				case FbxLayerElement::eDirect:
					materialIndex = polygonIdx;
					break;
				// 머티리얼이 인덱스로 매핑되어있는 경우
				case FbxLayerElement::eIndex:
				case FbxLayerElement::eIndexToDirect:
					materialIndex = layer.VertexMaterialSets[0]->GetIndexArray().GetAt(polygonIdx);
					break;
				}
				break;
			default:
				break;
			}

			// triangle poly를 우리의 Vertex구조에 맞도록 채워 넣어야 함 (삼각형이므로 faceCount는 1임 순회는 1번만 함)
			int32_t vertexIndices[3];
			for (int32_t triangleIndex = 0; triangleIndex < polygonFaceCount; ++triangleIndex)
			{
				// fbx sceneSetting이 오른손 z-up이므로 왼손좌표계에 맞게 변환
				vertexIndices[0] = 0;
				vertexIndices[1] = triangleIndex + 2;
				vertexIndices[2] = triangleIndex + 1;

				int32_t cornerIndices[3] =
				{
					fbxMesh->GetPolygonVertex(polygonIdx, vertexIndices[0]),
					fbxMesh->GetPolygonVertex(polygonIdx, vertexIndices[1]),
					fbxMesh->GetPolygonVertex(polygonIdx, vertexIndices[2])
				};

				int32_t uv[3] =
				{
					fbxMesh->GetTextureUVIndex(polygonIdx, vertexIndices[0]),
					fbxMesh->GetTextureUVIndex(polygonIdx, vertexIndices[1]),
					fbxMesh->GetTextureUVIndex(polygonIdx, vertexIndices[2])
				};

				FTriangle<Vertex::FVertexInfo_Base> triangle;
				triangle.SubIndex = materialIndex;

				FbxAMatrix vertexMat; // 정점의 변환 행렬
				FbxAMatrix normalMat; // 노말의 변환 행렬
				{
					// 노드의 기하학적 변환 행렬을 가져온다.
					// 노드의 기하학적 변환 행렬을 가져오는 이유는 노드의 기하학적 변환에 따라 정점도 변환되어야 하기 때문이다.
					FbxVector4 translation = InNode->GetGeometricTranslation(FbxNode::eSourcePivot); // 노드의 기하학적 변환
					FbxVector4 rotation    = InNode->GetGeometricRotation(FbxNode::eSourcePivot); // 노드의 기하학적 회전
					FbxVector4 scale       = InNode->GetGeometricScaling(FbxNode::eSourcePivot); // 노드의 기하학적 스케일

					FbxAMatrix geometryMat; // 노드의 기하학적 변환 행렬
					geometryMat.SetT(translation); // 변환 행렬 설정
					geometryMat.SetR(rotation); // 회전 행렬 설정
					geometryMat.SetS(scale); // 스케일 행렬 설정

					// 노드의 기하학적 변환 행렬을 가져와서 정점의 위치, 노말의 방향 정보에 변환 행렬을 적용하면 노드의 기하학적 변환에 따라 정점의 위치, 노말의 방향이 변환된다.
					vertexMat = geometryMat; // 정점의 변환 행렬설정
					// 노말(법선)의 변환 행렬은 기하학적 변환 행렬의 역행렬전치하면 된다.
					normalMat = vertexMat; // 노말의 변환 행렬 설정
					normalMat = normalMat.Inverse(); // 역행렬로 변환
					normalMat = normalMat.Transpose(); // 전치행렬로 변환
				}


				// 삼각형의 각 정점을 순회하면서 정점 정보를 파싱
				for (int32_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
				{
					const int32_t&   dccIndex = cornerIndices[vertexIndex];
					const FbxVector4 curVert  = vertices[dccIndex];

					FbxVector4 finalPosition;
					FbxVector4 finalNormal;
					FbxVector4 finalTangent;
					FbxVector4 finalBinormal;
					FbxVector2 finalUV;
					FbxColor   finalColor(1, 1, 1, 1);

					// Position
					finalPosition = vertexMat.MultT(curVert);

					// Normal
					if (!layer.VertexNormalSets.empty())
						finalNormal = ReadNormal(fbxMesh,
												 layer.VertexNormalSets.size(),
												 layer.VertexNormalSets[0],
												 dccIndex,
												 curPolyIndex + vertexIndices[vertexIndex]
												);
					finalNormal = normalMat.MultT(finalNormal);
					finalNormal.Normalize();

					if (!layer.VertexTangentSets.empty())
						finalTangent = ReadTangent(fbxMesh,
												   layer.VertexNormalSets.size(),
												   layer.VertexTangentSets[0],
												   dccIndex,
												   curPolyIndex + vertexIndices[vertexIndex]);
					finalTangent = normalMat.MultT(finalTangent);
					finalTangent.Normalize();

					if (!layer.VertexBinormalSets.empty())
						finalBinormal = ReadBinormal(fbxMesh,
													 layer.VertexNormalSets.size(),
													 layer.VertexBinormalSets[0],
													 dccIndex,
													 curPolyIndex + vertexIndices[vertexIndex]
													);
					finalBinormal = normalMat.MultT(finalBinormal);
					finalBinormal.Normalize();

					if (!layer.VertexUVSets.empty())
						finalUV = ReadUV(fbxMesh,
										 layer.VertexUVSets.size(),
										 layer.VertexUVSets[0],
										 uv[vertexIndex],
										 dccIndex);

					// Vertex Color
					if (!layer.VertexColorSets.empty())
						finalColor = ReadColor(fbxMesh,
											   layer.VertexColorSets.size(),
											   layer.VertexColorSets[0],
											   curPolyIndex + vertexIndices[vertexIndex],
											   dccIndex);

					// 저장될 vertexInfo
					Vertex::FVertexInfo_Base vertex;
					{
						vertex.Position.x = static_cast<float>(finalPosition.mData[0]);
						vertex.Position.y = static_cast<float>(finalPosition.mData[2]);
						vertex.Position.z = static_cast<float>(finalPosition.mData[1]);

						vertex.Normal.x = static_cast<float>(finalNormal.mData[0]);
						vertex.Normal.y = static_cast<float>(finalNormal.mData[2]);
						vertex.Normal.z = static_cast<float>(finalNormal.mData[1]);

						vertex.UV.x = static_cast<float>(finalUV.mData[0]);
						vertex.UV.y = 1.f - static_cast<float>(finalUV.mData[1]);

						vertex.Tangent = static_cast<float>(finalTangent.mData[0]);
						vertex.Tangent = static_cast<float>(finalTangent.mData[2]);
						vertex.Tangent = static_cast<float>(finalTangent.mData[1]);

						vertex.Binormal = static_cast<float>(finalBinormal.mData[0]);
						vertex.Binormal = static_cast<float>(finalBinormal.mData[2]);
						vertex.Binormal = static_cast<float>(finalBinormal.mData[1]);

						vertex.Color.x = static_cast<float>(finalColor.mRed);
						vertex.Color.y = static_cast<float>(finalColor.mGreen);
						vertex.Color.z = static_cast<float>(finalColor.mBlue);
						vertex.Color.w = static_cast<float>(finalColor.mAlpha);
					}
					// 삼각형에 정점정보 하나를 넣는다.
					// 이 삼각형배열로 IndexArray를 만들고 Index로 Draw
					triangle.Vertex[vertexIndex] = vertex;
				}

				auto& subMeshes = InMeshData->mSubMesh;
				// 서브메시가 존재하면 (메시안에 머티리얼이 여러개면)
				if (!subMeshes.empty())
				{
					// 현재 머티리얼과 일치하는 서브메시에 삼각형을 넣는다.
					subMeshes[materialIndex]->mVertexData->TriangleList.push_back(triangle);
					subMeshes[materialIndex]->mVertexData->FaceCount++;
				}
				else
				{
					InMeshData->mVertexData->TriangleList.push_back(triangle);
				}
				InMeshData->mVertexData->FaceCount++;
			}
			curPolyIndex += polygonSize;
		}
	}

	FLayer FbxFile::ParseMeshLayer(FbxMesh* InMesh, const Ptr<JMesh>& InMeshData)
	{
		FLayer layer;

		if (!InMesh)
			return layer;

		const int32_t layerCount = InMesh->GetLayerCount();

		// Normal없으면 Normal 데이터 생성
		if (layerCount == 0 || !InMesh->GetLayer(0)->GetNormals())
		{
			assert(FBXSDK_VERSION_MAJOR >= 2015);

			InMesh->InitNormals();
			InMesh->GenerateNormals();
		}

		// Tangent없으면 생성
		if (!InMesh->GetLayer(0)->GetTangents())
		{
			// InitTangents()를 호출할 필요는 없다고 한다...
			// GenerateTangentsData()에 내부적으로 초기화 작업이 있음
			InMesh->GenerateTangentsData(0);
		}

		std::vector<Ptr<JMaterial>> materials;

		/// 레이어별로 Normal, Tangent, Color, UV, 머티리얼(정점에 다수의 텍스처가 매핑 되어있을 경우) 있으면 정보를 넣어놓는다.
		/// 보통은 Layer0에만 존재하는데, 
		/// 예외로 머티리얼, UV는 Layer가 여러개일 수도 있다.
		for (int32_t layerIndex = 0; layerIndex < layerCount; ++layerIndex)
		{
			FbxLayer* curLayer = InMesh->GetLayer(layerIndex);

			if (FbxLayerElementVertexColor* vertexColor = curLayer->GetVertexColors())
			{
				layer.VertexColorSets.push_back(vertexColor);
			}
			if (FbxLayerElementUV* uv = curLayer->GetUVs())
			{
				layer.VertexUVSets.push_back(uv);
			}
			if (FbxLayerElementNormal* normal = curLayer->GetNormals())
			{
				layer.VertexNormalSets.push_back(normal);
			}
			if (FbxLayerElementTangent* tangent = curLayer->GetTangents())
			{
				layer.VertexTangentSets.push_back(tangent);
			}
			if (FbxLayerElementBinormal* binormal = curLayer->GetBinormals())
			{
				layer.VertexBinormalSets.push_back(binormal);
			}
			if (FbxLayerElementMaterial* material = curLayer->GetMaterials())
			{
				layer.VertexMaterialSets.push_back(material);

				const int32_t layerMaterialNum = material->mDirectArray->GetCount();

				/// 다수의 머티리얼이 존재하면 sub-mesh(기존과 다른 vertexBuffer를 생성)가 필요하다.
				/// SubMesh를 통해 서로 다른 머티리얼을 가진 메시를 하나의 메시로 표현할 수 있다.
				if (layerMaterialNum > 1)
				{
					for (int32_t i = 0; i < layerMaterialNum; ++i)
					{
						Ptr<JMaterial> fbxMat = Utils::Fbx::ParseLayerMaterial(InMesh, i);
						materials.push_back(fbxMat);

						// 서브메시를 만들자. (자세한 정보는 나중에 채워질 것)
						// 현재는 머티리얼이 여러개 있으니 서브메시를 머티리얼 개수만큼 생성한다.
						auto subMesh = MakePtr<JMesh>();
						// 서브메시에도 마찬가지로 VertexData를 생성해야 한다.
						subMesh->mVertexData = MakePtr<JData<Vertex::FVertexInfo_Base>>();

						// 현재 메시에 서브메시를 추가한다.
						InMeshData->mSubMesh.push_back(subMesh);

						// 머티리얼이 몇개나 있는거여?
						InMeshData->mMaterialRefNum++;
					}
				}
				// 메시당 하나의 머티리얼만을 사용한다면 submesh를 만들 필요가 없어진다.
				else
				{
					InMeshData->mMaterialRefNum = 1;

					// 머티리얼 정보만 가져와서 저장해놓자.
					Ptr<JMaterial> fbxMat = Utils::Fbx::ParseLayerMaterial(InMesh, 0);
					materials.push_back(fbxMat);
				}
			}

			layer.Layer = curLayer;
		}

		mMaterialList.push_back(materials);

		// // 메시에 몇개의 머티리얼이 사용되었는지 저장
		// InMeshData->mMaterialRefNum = mMaterialList.size();

		return layer;
	}

}
