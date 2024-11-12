#include "FbxFile.h"
#include "FbxUtils.h"
#include "Core/Entity/Animation/JAnimationClip.h"
#include "Core/Graphics/DirectMathHelper.h"
#include "Core/Graphics/Material/Instance/JMaterialInstance.h"
#include "Core/Graphics/Mesh/JMeshObject.h"
#include "Core/Graphics/Mesh/JMeshData.h"
#include "Core/Graphics/Mesh/JSkeletalMeshObject.h"
#include "Core/Interface/MManagerInterface.h"

namespace Utils::Fbx
{
	FbxManager* g_fbx_manager = nullptr;

	FbxFile::~FbxFile()
	{
		Release_Internal();
	}

	void FbxFile::Initialize(const char* InFilePath)
	{
		// fbx를 로드할 매니저가 초기화 되어있지 않았으면 초기화
		if (!g_fbx_manager)
		{
			g_fbx_manager = FbxManager::Create();
		}
		assert(g_fbx_manager);

		// 기존에 생성된 FbxScene, FbxImporter가 존재하면 제거
		if (mFbxScene)
		{
			mFbxScene->Destroy();
		}
		if (mFbxImporter)
		{
			mFbxImporter->Destroy();
		}

		// fbx sdk에서는 Load할 때 Scene으로 로드가 된다 (블렌더에서 Scene안에 여러 오브젝트가 있을 수 있고 라이트, 카메라등이 있을 수 있음)
		mFbxImporter = FbxImporter::Create(g_fbx_manager, "");
		mFbxScene    = FbxScene::Create(g_fbx_manager, "");
		assert(mFbxImporter);
		assert(mFbxScene);

		bool bSuccess = mFbxImporter->Initialize(InFilePath, -1, g_fbx_manager->GetIOSettings());
		if (!bSuccess)
		{
			FbxStatus status = mFbxImporter->GetStatus();
			LOG_CORE_ERROR("Failed to initialize fbx importer, Error: {0}, {1}, {2}",
						   status.GetErrorString(),
						   __FILE__,
						   __LINE__);
			return;
		}


		bSuccess = mFbxImporter->Import(mFbxScene);
		if (!bSuccess)
		{
			LOG_CORE_ERROR("Failed to import fbx importer, {0} {1}", __FILE__, __LINE__);
			return;
		}

		// Importer같은경우 Import이후에는 FbxScene메모리에 데이터가 존재하므로 Destroy해도 무방
		mFbxImporter->Destroy();

		// front -> y | right -> x | up -> z (y, z 축이 바뀐 형태) 
		FbxAxisSystem::MayaZUp.ConvertScene(mFbxScene);
		FbxSystemUnit::cm.ConvertScene(mFbxScene);

		mMeshList.clear();
		mMaterialList.clear();
		mNumVertex = 0;
		mNumIndex  = 0;
	}

	void FbxFile::Release_Internal() const
	{
		if (mFbxScene)
			mFbxScene->Destroy();
	}

	void FbxFile::Release()
	{
		if (g_fbx_manager)
			g_fbx_manager->Destroy();
	}

	bool FbxFile::Load(const char* InFilePath)
	{
		// 로드 전 sdk 초기화
		Initialize(InFilePath);

		// 실제 파싱 작업
		ProcessLoad();

		// 파싱된 데이터 메모리에 저장
		Convert();

		// 데이터 저장
		if (!SaveMeshData(InFilePath) || !SaveAnimationData(InFilePath))
		{
			return false;
		}

		return true;
	}

	bool FbxFile::Convert()
	{
		for (int32_t meshIndex = 0; meshIndex < mMeshList.size(); ++meshIndex)
		{
			auto& mesh = mMeshList[meshIndex];
			auto& data = mesh->mVertexData;

			mesh->mIndex     = meshIndex;
			mesh->mFaceCount = data->FaceCount;

			// 부모 노드 존재시에 자식 노드로 등록
			if (Ptr<JMeshData> parentMesh = mesh->mParentMesh.lock())
			{
				parentMesh->mChildMesh.push_back(mesh);
			}

			// 메시 타입이 Static(Static Mesh)일 경우만 우선 처리
			const int32_t materialNum = mesh->mMaterialRefNum - 1;
			assert(materialNum >= 0);

			JArray<Ptr<JMaterialInstance>> materials = mMaterialList[meshIndex];
			if (!materials.empty())
			{
				// 머티리얼이 한개일 경우 서브메시를 사용하지 않는다.
				if (materials.size() == 1)
				{
					Ptr<JMaterialInstance> subMaterial = materials[0];

					// 단일 메시를 생성한다 (실제 생성이 아니라 정점, 인덱스 배열을 생성).
					data->GenerateIndexArray(data->TriangleList);

					mesh->mMaterialInstance = subMaterial;

					mNumVertex += data->VertexArray.size();
					mNumIndex += data->IndexArray.size();
				}
				else
				{
					for (int subMeshIndex = 0; subMeshIndex < mesh->mSubMesh.size();)
					{
						Ptr<JMaterialInstance> subMaterial = materials[subMeshIndex];

						auto& subMesh = mesh->mSubMesh[subMeshIndex];
						auto& subData = subMesh->mVertexData;

						if (subData->TriangleList.empty())
						{
							mesh->mSubMesh.erase(mesh->mSubMesh.begin() + subMeshIndex);
							continue;
						}

						subData->GenerateIndexArray(subData->TriangleList);

						subMesh->mMaterialInstance = subMaterial;

						mNumVertex += data->VertexArray.size();
						mNumIndex += data->IndexArray.size();

						const int32_t faceCount = subData->IndexArray.size() / 3;
						subData->FaceCount      = faceCount;
						subMesh->mFaceCount     = faceCount;

						mesh->mFaceCount += faceCount;

						++subMeshIndex;
					}
				}
			}
		}


		return true;
	}


	bool FbxFile::SaveMeshData(const char* InFilePath)
	{
		if (!mMeshList.empty())
		{
			JText filePath = std::format("Game/Mesh/{0}.jasset", ParseFile(InFilePath));

			const auto object = bHasSkeleton
									? MakePtr<JSkeletalMeshObject>(filePath, mMeshList)
									: MakePtr<JMeshObject>(filePath, mMeshList);

			if (!Serialization::Serialize(filePath.c_str(), object.get()))
			{
				LOG_CORE_ERROR("Failed to serialize mesh object, {0} {1}", __FILE__, __LINE__);
				return false;
			}
		}

		return true;
	}

	bool FbxFile::SaveAnimationData(const char* InFilePath) const
	{
		if (!mAnimations.empty())
		{
			for (int32_t i = 0; i < mAnimations.size(); ++i)
			{
				auto& clip     = mAnimations[i];
				JText filePath = std::format("Game/Animation/{0}.jasset", ParseFile(InFilePath));

				if (!Serialization::Serialize(filePath.c_str(), clip.get()))
				{
					LOG_CORE_ERROR("Failed to serialize animation clip, {0} {1}", __FILE__, __LINE__);
					return false;
				}
			}
		}

		return true;
	}

	void FbxFile::ProcessLoad()
	{
		FbxNode* root = mFbxScene->GetRootNode();
		assert(root, "empty scene(node x)");

		// 본 정보 파싱
		ParseSkeleton_Recursive(root, 1, 0);

		// Parse Mesh
		ParseNode_Recursive(root, FbxNodeAttribute::EType::eMesh);

		ParseAnimation(mFbxScene);
	}

	void FbxFile::ParseSkeleton_Recursive(FbxNode* InNode, int32_t InIndex, int32_t InParentIndex)
	{
		if (!InNode)
			return;

		if (InNode->GetNodeAttribute() && InNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			FJointData jointData;
			{
				jointData.Name        = InNode->GetName();
				jointData.ParentIndex = mSkeletonData.Joints.empty() ? -1 : InParentIndex;
			}

			mSkeletonData.Joints.push_back(jointData);

			if (bHasSkeleton == false)
			{
				bHasSkeleton = true;
			}
		}

		const int32_t childCount = InNode->GetChildCount();
		for (int32_t i = 0; i < childCount; ++i)
		{
			ParseSkeleton_Recursive(InNode->GetChild(i), mSkeletonData.Joints.size(), InIndex);
		}
	}

	void FbxFile::ParseNode_Recursive(FbxNode*              InNode, FbxNodeAttribute::EType  NodeAttribute,
									  const Ptr<JMeshData>& InParentMeshData, const FMatrix& InParentMatrix)
	{
		if (!InNode)
			return;

		/// 모델링 툴에서 보면 알겠지만 노드는 메시, 라이트, 카메라, 스켈레톤 등 엄청나게 많은 타입이 있다.
		/// 이 노드 어떤 타입인지는 GetAttributeType()로 확인할 수 있고
		/// 개체를 얻으려면 GetAttribute로 구하여 reinterpret_cast로 캐스팅하면 된다.
		const FbxNodeAttribute* attribute = InNode->GetNodeAttribute();

		/// 노드가 다른 타입이라도 자식 노드의 타입이 일치한다면 부모노드의 변환 행렬을 전달한다.
		FMatrix worldMat = ParseTransform(InNode, InParentMatrix);

		/// 현재 노드가 파싱하려는 노드 타입과 일치하면 파싱을 시작한다.
		/// 즉, 파싱된 정보를 담을 MeshData를 생성
		Ptr<JMeshData> meshData = nullptr;
		if (attribute && attribute->GetAttributeType() == NodeAttribute)
		{
			meshData = bHasSkeleton ? MakePtr<JSkeletalMesh>() : MakePtr<JMeshData>();

			{
				meshData->mName                  = InNode->GetName();
				meshData->mParentMesh            = InParentMeshData;
				meshData->mMaterialRefNum        = 0;
				meshData->mFaceCount             = 0;
				meshData->mVertexData            = MakePtr<JVertexData<Vertex::FVertexInfo_Base>>();
				meshData->mInitialModelTransform = worldMat;
			}
			mMeshList.push_back(meshData);

			switch (NodeAttribute)
			{
			case FbxNodeAttribute::eMesh:
				ParseMesh(InNode, meshData);
				break;
			default:
				break;
			}
		}

		// 자식 노드를 재귀적으로 파싱
		for (int32_t i = 0; i < InNode->GetChildCount(); ++i)
		{
			ParseNode_Recursive(InNode->GetChild(i), NodeAttribute, meshData, worldMat);
		}
	}


	void FbxFile::ParseMesh(FbxNode* InNode, Ptr<JMeshData> InMeshData)
	{
		if (!InNode || !InNode->GetMesh())
			return;

		// fbx에서 정의된 메시를 가져오자.
		FbxMesh* fbxMesh = InNode->GetMesh();

		// DeFormer & Skin
		Ptr<JSkinData> skinData = MakePtr<JSkinData>();
		ParseMeshSkin(fbxMesh, skinData.get());

		int32_t boneCount = skinData->GetBoneCount();
		if (boneCount > 0)
		{
			InMeshData->SetSkinData(skinData);
			InMeshData->SetSkeletalData(mSkeletonData);
			InMeshData->mClassType = EMeshType::Skeletal;

			for (int32_t i = 0; i < boneCount; ++i)
			{
				JText boneName = skinData->GetInfluenceBoneName(i);
				InMeshData->AddInfluenceBone(boneName, skinData->GetInfluenceBoneInverseBindPose(boneName));
			}
		}
		else
		{
			InMeshData->mClassType = EMeshType::Static;
		}

		// 레이어 정보(Geometry (UV, Tangents, NormalMap, Material, Color...))부터 해석하자.
		FLayer layer = ParseMeshLayer(fbxMesh, InMeshData);

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

				FTriangle<Vertex::FVertexInfo_Base> triangle(materialIndex);

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


					// NormalMap
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
						// Get Scale Info
						FVector scale = Mat2ScaleVector(InMeshData->mInitialModelTransform);
						vertex.Position *= scale;

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

						if (bHasSkeleton)
						{
							int32_t* indices     = skinData->GetBoneIndex(dccIndex);
							vertex.BoneIndices.x = static_cast<uint32_t>(indices[0]);
							vertex.BoneIndices.y = static_cast<uint32_t>(indices[1]);
							vertex.BoneIndices.z = static_cast<uint32_t>(indices[2]);
							vertex.BoneIndices.w = static_cast<uint32_t>(indices[3]);

							float* weights       = skinData->GetBoneWeight(dccIndex);
							vertex.BoneWeights.x = weights[0];
							vertex.BoneWeights.y = weights[1];
							vertex.BoneWeights.z = weights[2];
							vertex.BoneWeights.w = weights[3];
						}
					}

					// 삼각형에 정점정보 하나를 넣는다.
					// 이 삼각형배열로 IndexArray를 만들고 Index로 Draw
					triangle.Vertex[vertexIndex] = vertex;
				}

				auto& subMeshes = InMeshData->mSubMesh;
				// 서브메시가 존재하면 (메시안에 머티리얼이 여러개면)
				if (!subMeshes.empty())
				{
					subMeshes[materialIndex]->mVertexData->TriangleList.push_back(triangle);
					subMeshes[materialIndex]->mVertexData->FaceCount++;
				}
				else
				{
					InMeshData->mVertexData->TriangleList.push_back(triangle);
					InMeshData->mVertexData->FaceCount++;
				}
			}
			curPolyIndex += polygonSize;
		}
	}

	void FbxFile::ParseMeshSkin(const FbxMesh* InMesh, JSkinData* InSkinData)
	{
		// 스켈레톤 데이터가 없으면 스키닝 정보를 파싱할 필요가 없다.
		// if (mSkeletonData.Joints.empty())
		// 	return;

		/// 메시에 붙어있는 스킨을 가져오자.
		/// 스킨(skinning)이라는게 뭘까?
		/// 쉽게 말하면 메시(Mesh)의 정점(Vertex)이 어떤 관절(Bone)에 영향을 받는지를 나타내는 정보이다.
		/// 그러니까 이전에 파싱한 스켈레톤과 정점과의 관계를 파악하는 것이다.
		/// 캐릭터 스켈레톤의 Hip본의 경우 어느 메시에 그리고 어느 정점에 영향을 주는지 알아야 한다.
		/// 그리고 정점마다 Hip본에 대한 영향을 가중치(weight)로 표현한다. (내가 Hip을 움직이면 Hip을 따라서 척추, 상체 등이 움직인다.)
		/// (따라서 가중치는 여러 본에서 영향을 받을 수 있다.)
		/// 메시 안에는 여러개의 스킨이 붙어있을 수 있으므로 순회하면서 스킨을 가져온다.

		// deformer는 Fbx와 관련된 것.
		// deformer는 메시의 변형을 관리하는 객체로, 스킨, 클러스터, 블렌드쉐이프 등이 있다.(여기서는 스킨만 사용)
		int32_t deformerCount = InMesh->GetDeformerCount(FbxDeformer::eSkin); // 보통 메시당 스킨이 하나만 붙어있을 것이다.
		if (deformerCount == 0)	// 스킨이 없으면
			return;


		// SkinData에 메시 정보를 집어넣자
		const int32_t     vertexCount  = InMesh->GetControlPointsCount();	// 영향을 받는 정점 개수
		constexpr int32_t vertexStride = 4;									// 가중치 용량 (몇개의 본에서 영향을 받나? 8개까지 넘어가는건 괴물 같은 메시)

		InSkinData->Initialize(vertexCount, vertexStride);

		JArray<FbxMatrix> sample;
		int               poseCount = mFbxScene->GetPoseCount();
		for (int i = 0; i < poseCount; i++)
		{
			FbxPose* pose = mFbxScene->GetPose(i);
			if (pose->IsBindPose())
			{  // 바인드 포즈인지 확인
				int nodeCount = pose->GetCount();
				for (int j = 0; j < nodeCount; j++)
				{
					FbxNode* node = pose->GetNode(j);
					if (node)
					{
						LOG_CORE_INFO("Bind Pose Node Name: {0}", node->GetName());
						FbxMatrix bindPoseMatrix = pose->GetMatrix(j);
						sample.push_back(bindPoseMatrix);
					}
				}
			}
		}

		// 스킨 갯수 만큼 순회
		for (int32_t deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
		{
			// 우리는 디포머에서 스킨만 사용할 것이다.
			FbxSkin* skin = reinterpret_cast<FbxSkin*>(InMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));

			// 클러스터는 특정본에 의해 영향을 받는 정점들의 집합이다.
			// 클러스터에는 link(joint)와 weight가 있다.
			const int32_t clusterCount = skin->GetClusterCount(); // 본(관절) 개수

			// 각 joint(관절) 순회
			for (int32_t clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
			{
				FbxCluster*    cluster   = skin->GetCluster(clusterIndex);
				const FbxNode* joint     = cluster->GetLink();
				JText          jointName = joint->GetName();

				const int32_t  clusterSize = cluster->GetControlPointIndicesCount();	// 이 본에 영향을 받는 인덱스 배열 크기
				const int32_t* indices     = cluster->GetControlPointIndices();			// 이 본에 영향을 받는 인덱스 배열
				const double*  weights     = cluster->GetControlPointWeights();			// 이 본에 영향을 받는 가중치 배열
				const int32_t  boneIndex   = InSkinData->GetBoneCount();				// 이 본의 인덱스

				/// BindPose?
				/// 이 FbxSdk는 autodesk Maya에서 사용되는 기능으로 용어가 정립이 되어있는거같은데
				/// 보통 스켈레톤(본)과 메시를 연결되기 위한 기본 포즈이다.
				/// 예로 캐릭터 메시가 T-Pose로 되어있다면 본 또한 T-Pose로 되어있을 때 이걸 연결시키고 둘을 Binding한다.
				FbxAMatrix boneBindPose;		// 본의 변환 행렬 (좌표: 월드 좌표계)
				FbxAMatrix vertexGroupBindPose;	// 메시(VertexGroup, 클러스터)의 변환 행렬 (좌표: 월드 좌표계)

				cluster->GetTransformLinkMatrix(boneBindPose);		// 본의 변환 행렬 
				cluster->GetTransformMatrix(vertexGroupBindPose);	// 메시(VertexGroup, 클러스터)의 변환 행렬

				InSkinData->AddBindPose(jointName, Utils::Fbx::Maya2DXMat(FMat2JMat(boneBindPose)));

				// BindPoseMatrix를 저장한다.
				FbxMatrix bindPosMat =
						vertexGroupBindPose.Inverse()		// 메시 로컬 좌표계로 변환위해 역행렬
						* boneBindPose;						// 본의 변환 행렬을 메시 로컬 좌표계로 변환

				CaptureBindPoseMatrix(InSkinData, joint, bindPosMat);

				InSkinData->AddInfluenceBone(joint->GetName());

				// 영향받는 정점들을 순회하면서 가중치를 넣는다.
				for (int32_t i = 0; i < clusterSize; ++i)
				{
					const int32_t vertexIndex = indices[i];
					const float   weight      = static_cast<float>(weights[i]);
					InSkinData->AddWeight(vertexIndex, boneIndex, weight);
				}
			}
		}


	}

	void FbxFile::ParseAnimation(FbxScene* InScene)
	{
		assert(InScene);

		// 애니메이션 스택(여러개 애니메이션을 가질 수 있다.)
		// 근데 대부분 우리가 사용할 Fbx에서는 개별적인 애니메이션을 임포트 할 거긴하다.
		// 게세키로를 변환하면 그렇게 됨... (X 우리는 언리얼에서 하나씩 임포트할것임)
		FbxArray<FbxString*> animStackNameArray;
		InScene->FillAnimStackNameArray(animStackNameArray);

		// 애니메이션 갯수만큼 순회
		const int32_t animStackCount = animStackNameArray.GetCount();
		for (int32_t i = 0; i < animStackCount; ++i)
		{
			ParseAnimationStack(InScene, animStackNameArray.GetAt(i));
		}
	}

	void FbxFile::ParseAnimationStack(FbxScene* Scene, FbxString* AnimStackName)
	{
		const FbxAnimStack* animStack = Scene->FindMember<FbxAnimStack>(AnimStackName->Buffer());
		if (!animStack)
			return;

		Scene->GetAnimationEvaluator()->Reset();

		// 얘네는 애니메이션 클립을 TakeInfo라고 부른다.
		const FbxTakeInfo* animClip = Scene->GetTakeInfo(animStack->GetName());
		if (!animClip)
			return;

		// 1프레임을 기준으로 시간을 설정 (우리는 프레임 단위로 애니메이션의 변환 행렬을 가져와야 함.)
		FbxTime frameTime;
		frameTime.SetTime(0, 0, 0, 1, 0, Scene->GetGlobalSettings().GetTimeMode());

		// 1프레임의 길이를 초 단위로 변환
		float frameTimeSec = static_cast<float>(frameTime.GetSecondDouble());
		// 1 프레임 간격
		float frameStep  = 1.f;
		float sampleTime = frameTimeSec * frameStep;
		assert(sampleTime > 0);

		// 애니메이션의 시작, 끝 시간을 가져온다.
		float startTime = static_cast<float>(animClip->mLocalTimeSpan.GetStart().GetSecondDouble());
		float endTime   = static_cast<float>(animClip->mLocalTimeSpan.GetStop().GetSecondDouble());

		Ptr<JAnimationClip> anim      = MakePtr<JAnimationClip>();
		anim->mName                   = animClip->mName.Buffer();
		anim->mStartTime              = startTime;
		anim->mEndTime                = endTime;
		anim->mSourceSamplingInterval = sampleTime;

		mAnimations.push_back(anim);

		ParseAnimNode(Scene->GetRootNode(), -1, true);

		const int32_t nodeCount = mScanList.size();
		for (int32_t i = 0; i < nodeCount; ++i)
		{
			const char*         trackName = mScanList[i].Node->GetName();
			Ptr<JAnimBoneTrack> animTrack = MakePtr<JAnimBoneTrack>();
			animTrack->Name               = trackName;
			anim->AddTrack(animTrack);
			mScanList[i].AnimationTrack = animTrack;
		}

		CaptureAnimation(anim, Scene);
		anim->OptimizeKeys();
	}

	void FbxFile::ParseAnimNode(FbxNode* InNode, int32_t InParentIndex, bool bSkeletalAnim)
	{
		const int32_t parentIndex = mScanList.size();
		const int32_t childNum    = InNode->GetChildCount();

		FAnimationNode animNode{};
		animNode.ParentIndex = mScanList.empty() ? -1 : InParentIndex;
		animNode.Node        = InNode;


		if (bSkeletalAnim)
		{
			if (InNode->GetNodeAttribute() && InNode->GetNodeAttribute()->GetAttributeType() ==
				FbxNodeAttribute::eSkeleton)
			{
				mScanList.push_back(animNode);
			}
		}
		else
		{
			mScanList.push_back(animNode);
		}


		for (int32_t i = 0; i < childNum; ++i)
		{
			ParseAnimNode(InNode->GetChild(i), parentIndex, bSkeletalAnim);
		}
	}

	FLayer FbxFile::ParseMeshLayer(FbxMesh* InMesh, const Ptr<JMeshData>& InMeshData)
	{
		FLayer layer;

		if (!InMesh)
			return layer;

		const int32_t layerCount = InMesh->GetLayerCount();

		// Normal없으면 NormalMap 데이터 생성
		if (layerCount == 0 || !InMesh->GetLayer(0)->GetNormals())
		{
			// 2015 이상 버전에서만 사용 가능
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

		JArray<Ptr<JMaterialInstance>> materials;

		/// 레이어별로 NormalMap, Tangent, Color, UV, 머티리얼(정점에 다수의 텍스처가 매핑 되어있을 경우) 있으면 정보를 넣어놓는다.
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

				const int32_t layerMaterialNum = InMesh->GetNode()->GetMaterialCount();

				const JText saveFilePath     = std::format("Game/Materials/{0}", InMesh->GetName());
				bool        bShouldSerialize = false;

				/// 다수의 머티리얼이 존재하면 sub-mesh(기존과 다른 vertexBuffer를 생성)가 필요하다.
				/// SubMesh를 통해 서로 다른 머티리얼을 가진 메시를 하나의 메시로 표현할 수 있다.
				if (layerMaterialNum > 1)
				{
					for (int32_t i = 0; i < layerMaterialNum; ++i)
					{
						Ptr<JMaterialInstance> materialInstance = ParseLayerMaterial(InMesh, i, bShouldSerialize);
						materials.push_back(materialInstance);

						// 서브메시를 만들자. (자세한 정보는 나중에 채워질 것)
						// 현재는 머티리얼이 여러개 있으니 서브메시를 머티리얼 개수만큼 생성한다.
						auto subMesh = MakePtr<JMeshData>();

						// subMesh의 이름을 정하는게 좀 애매한데
						// 우선 메시 이름 + 머티리얼 이름 으로 설정
						subMesh->mName = std::format("{0}_{1}",
													 InMeshData->mName,
													 materialInstance->GetMaterialName());

						// 서브메시에도 마찬가지로 VertexData를 생성해야 한다.
						subMesh->mVertexData = MakePtr<JVertexData<Vertex::FVertexInfo_Base>>();

						// 현재 메시에 서브메시를 추가한다.
						InMeshData->mSubMesh.push_back(subMesh);

						// 머티리얼 수 만큼 증가
						InMeshData->mMaterialRefNum++;

						if (bShouldSerialize)
						{
							if (!std::filesystem::exists(saveFilePath))
							{
								std::filesystem::create_directories(saveFilePath);
							}
							Utils::Serialization::Serialize(materialInstance->GetMaterialPath().c_str(),
															materialInstance.get());
						}
					}
				}
				// 메시당 하나의 머티리얼만을 사용한다면 submesh를 만들 필요가 없어진다.
				else
				{
					InMeshData->mMaterialRefNum = 1;

					// 머티리얼 정보만 가져와서 저장해놓자.
					Ptr<JMaterialInstance> materialInstance = ParseLayerMaterial(InMesh, 0, bShouldSerialize);
					materials.push_back(materialInstance);

					if (bShouldSerialize)
					{
						if (!std::filesystem::exists(saveFilePath))
						{
							std::filesystem::create_directories(saveFilePath);
						}
						Utils::Serialization::Serialize(materialInstance->GetMaterialPath().c_str(),
														materialInstance.get());
					}
				}
			}

			layer.Layer = curLayer;
		}

		mMaterialList.push_back(materials);

		// // 메시에 몇개의 머티리얼이 사용되었는지 저장
		// InMeshData->mMaterialRefNum = mMaterialList.size();

		return layer;
	}

	void FbxFile::CaptureBindPoseMatrix(JSkinData* Ptr, const FbxNode* Joint, const FbxMatrix& InBindPosMat)
	{
		FMatrix bindPoseMat = Utils::Fbx::Maya2DXMat(FMat2JMat(InBindPosMat));
		FMatrix bindPoseInvMat;
		bindPoseMat.Invert(bindPoseInvMat);

		Ptr->AddInverseBindPose(Joint->GetName(), bindPoseInvMat);
	}

	void FbxFile::CaptureAnimation(const Ptr<JAnimationClip>& Ptr, FbxScene* Scene)
	{
		const float deltaTime = Ptr->mSourceSamplingInterval;
		const float startTime = Ptr->mStartTime;
		const float endTime   = Ptr->mEndTime;

		float currentTime = startTime;

		const int32_t tracks = mScanList.size();

		// 시작부터 끝까지 시간을 증가시키면서 애니메이션 트랙정보를 캡쳐한다.
		while (currentTime <= endTime)
		{
			FbxTime time;
			time.SetSecondDouble(currentTime);

			for (int32_t i = 0; i < tracks; ++i)
			{
				FAnimationNode& animNode = mScanList[i];

				auto animEvaluator = Scene->GetAnimationEvaluator();

				auto localMat = animEvaluator->GetNodeLocalTransform(animNode.Node, time);

				AddKey(animNode, localMat, currentTime - startTime);
			}
			currentTime += deltaTime;
		}
	}

	void FbxFile::AddKey(FAnimationNode& InNode, const FbxAMatrix& InLocalMat, float InTime)
	{
		FMatrix localMat = Utils::Fbx::Maya2DXMat(FMat2JMat(InLocalMat));
		InNode.Transform = localMat;

		XMVECTOR scale;
		XMVECTOR rotation;
		XMVECTOR translation;

		XMMatrixDecompose(&scale, &rotation, &translation, localMat);

		FVector  scaleVec;
		FVector4 rotationVec;
		FVector  translationVec;

		XMStoreFloat3(&scaleVec, scale);
		XMStoreFloat4(&rotationVec, rotation);
		XMStoreFloat3(&translationVec, translation);

		InNode.AnimationTrack->AddKey(InTime, translationVec, rotationVec, scaleVec);
	}

}
