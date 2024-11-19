#pragma once
#include <fbxsdk.h>

#include "Core/Entity/Animation/JAnimationClip.h"
#include "Core/Graphics/Material/JMaterial.h"
#include "Core/Graphics/Material/MMaterialInstanceManager.h"
#include "Core/Graphics/Material/MMaterialManager.h"
#include "Core/Utils/Logger.h"
#include "Core/Utils/Math/TMatrix.h"

namespace Utils::Fbx
{
	/**
	 * @brief FbxMesh의 Layer 정보를 담는 구조체 (FbxMesh같은 Geometry개체에 존재)
	 * - VertexColors
	 * - UVs
	 * - Normals
	 * - Tangents
	 * - Materials
	 */
	struct FLayer
	{
		FbxLayer* Layer = nullptr;

		std::vector<FbxLayerElementUV*>          VertexUVSets;
		std::vector<FbxLayerElementVertexColor*> VertexColorSets;
		std::vector<FbxLayerElementNormal*>      VertexNormalSets;
		std::vector<FbxLayerElementMaterial*>    VertexMaterialSets;
		std::vector<FbxLayerElementTangent*>     VertexTangentSets;
		std::vector<FbxLayerElementBinormal*>    VertexBinormalSets;
	};

	/** 임시 텍스처 추출 구조체 */
	struct FFbxProperty
	{
		const char*         FbxPropertyName;
		const char*         PropertyName;
		uint8_t             PostOperations;
		EMaterialParamValue ParamValue;
	};

	struct FAnimationNode
	{
		int32_t             ParentIndex;
		FbxNode*            Node;
		Ptr<JAnimBoneTrack> AnimationTrack;
		FMatrix             Transform;
	};

	/**
	 * 애니메이션 키 프레임 데이터 단위 (Linked List)
	 * 기본적인 구조로 월드 상의 위치 데이터 및 시간 데이터를 가지고 있다.
	 */
	struct FKeyFrameData
	{
		int64_t             Time;
		FbxAMatrix          WorldTransform;
		UPtr<FKeyFrameData> NextKeyFrameData = nullptr;
	};

	inline const FFbxProperty FbxMaterialProperties[] =
	{
		// Diffuse
		{
			FbxSurfaceMaterial::sDiffuse, NAME_MAT_Diffuse, 0, EMaterialParamValue::Texture2D
		},
		// {
		// 	FbxSurfaceMaterial::sDiffuseFactor, NAME_MAT_DiffuseF, 0, EMaterialParamType::DiffuseFactor,
		// 	EMaterialParamValue::Float
		// },

		// Normal
		{
			FbxSurfaceMaterial::sNormalMap, NAME_MAT_Normal, 0, EMaterialParamValue::Texture2D
		},

		// // Emissive
		// {
		// 	FbxSurfaceMaterial::sEmissive, NAME_MAT_Emissive, 0, EMaterialParamType::EmissiveColor,
		// 	EMaterialParamValue::Texture2D
		// },
		// {
		// 	FbxSurfaceMaterial::sEmissiveFactor, NAME_MAT_EmissiveF, 0, EMaterialParamType::EmissiveFactor,
		// 	EMaterialParamValue::Float
		// },
		//
		// // Specular
		// {
		// 	FbxSurfaceMaterial::sSpecular, NAME_MAT_Specular, 0, EMaterialParamType::SpecularColor,
		// 	EMaterialParamValue::Texture2D
		// },
		// {
		// 	FbxSurfaceMaterial::sSpecularFactor, NAME_MAT_SpecularF, 0, EMaterialParamType::SpecularFactor,
		// 	EMaterialParamValue::Float
		// },
		//
		// // Reflection
		// {
		// 	FbxSurfaceMaterial::sReflection, NAME_MAT_Reflection, 0, EMaterialParamType::ReflectionColor,
		// 	EMaterialParamValue::Texture2D
		// },
		// {
		// 	FbxSurfaceMaterial::sReflectionFactor, NAME_MAT_ReflectionF, 0, EMaterialParamType::ReflectionFactor,
		// 	EMaterialParamValue::Float
		// },
		//
		// // Ambient
		// {
		// 	FbxSurfaceMaterial::sAmbient, NAME_MAT_Ambient, 0, EMaterialParamType::AmbientColor,
		// 	EMaterialParamValue::Texture2D
		// },
		// {
		// 	FbxSurfaceMaterial::sAmbientFactor, NAME_MAT_AmbientF, 0, EMaterialParamType::AmbientFactor,
		// 	EMaterialParamValue::Float
		// },
		//
		// // Transparency
		// {
		// 	FbxSurfaceMaterial::sTransparentColor, NAME_MAT_Transparent, 1, EMaterialParamType::TransparentColor,
		// 	EMaterialParamValue::Texture2D
		// },
		// {
		// 	FbxSurfaceMaterial::sTransparencyFactor, NAME_MAT_TransparentF, 1, EMaterialParamType::TransparentFactor,
		// 	EMaterialParamValue::Float
		// },
		//
		// // Displacement
		// {
		// 	FbxSurfaceMaterial::sDisplacementColor, NAME_MAT_Displacement, 0, EMaterialParamType::DisplacementColor,
		// 	EMaterialParamValue::Texture2D
		// },
		// {
		// 	FbxSurfaceMaterial::sDisplacementFactor, NAME_MAT_DisplacementF, 0, EMaterialParamType::DisplacementFactor,
		// 	EMaterialParamValue::Float
		// },
	};

	[[nodiscard]] inline FbxMatrix GetNodeTransform(const FbxNode* InNode)
	{
		if (!InNode)
		{
			throw std::exception("empty node");
		}

		const FbxVector4 transform = InNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 rotation  = InNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 scale     = InNode->GetGeometricScaling(FbxNode::eSourcePivot);

		return FbxMatrix(transform, rotation, scale);
	}

	/** FBX SDK Matrix -> Jacob Engine Matrix */
	[[nodiscard]] inline FMatrix FMat2JMat(const FbxMatrix& InMatrix)
	{
		FMatrix resultMatrix;

		float*        dest = reinterpret_cast<float*>(&resultMatrix);
		const double* src  = reinterpret_cast<const double*>(&InMatrix);

		for (int32_t i = 0; i < 16; ++i)
		{
			dest[i] = static_cast<float>(src[i]);
		}

		return resultMatrix;
	}

	/** Maya (z-up) axis -> Directx axis*/
	[[nodiscard]] inline FMatrix Maya2DXMat(const FMatrix& InMatrix)
	{
		FMatrix returnMatrix;

		returnMatrix._11 = InMatrix._11;
		returnMatrix._12 = InMatrix._13;
		returnMatrix._13 = InMatrix._12;
		returnMatrix._21 = InMatrix._31;
		returnMatrix._22 = InMatrix._33;
		returnMatrix._23 = InMatrix._32;
		returnMatrix._31 = InMatrix._21;
		returnMatrix._32 = InMatrix._23;
		returnMatrix._33 = InMatrix._22;
		returnMatrix._41 = InMatrix._41;
		returnMatrix._42 = InMatrix._43;
		returnMatrix._43 = InMatrix._42;

		returnMatrix._14 = returnMatrix._24 = returnMatrix._34 = 0.0f;
		returnMatrix._44 = 1.0f;
		return returnMatrix;
	}

#pragma region Read Node Data

	[[nodiscard]] inline FbxString GetAttributeTypeName(FbxNodeAttribute::EType type)
	{
		switch (type)
		{
		case FbxNodeAttribute::eUnknown:
			return "unidentified";
		case FbxNodeAttribute::eNull:
			return "null";
		case FbxNodeAttribute::eMarker:
			return "marker";
		case FbxNodeAttribute::eSkeleton:
			return "skeleton";
		case FbxNodeAttribute::eMesh:
			return "mesh";
		case FbxNodeAttribute::eNurbs:
			return "nurbs";
		case FbxNodeAttribute::ePatch:
			return "patch";
		case FbxNodeAttribute::eCamera:
			return "camera";
		case FbxNodeAttribute::eCameraStereo:
			return "stereo";
		case FbxNodeAttribute::eCameraSwitcher:
			return "camera switcher";
		case FbxNodeAttribute::eLight:
			return "light";
		case FbxNodeAttribute::eOpticalReference:
			return "optical reference";
		case FbxNodeAttribute::eOpticalMarker:
			return "marker";
		case FbxNodeAttribute::eNurbsCurve:
			return "nurbs curve";
		case FbxNodeAttribute::eTrimNurbsSurface:
			return "trim nurbs surface";
		case FbxNodeAttribute::eBoundary:
			return "boundary";
		case FbxNodeAttribute::eNurbsSurface:
			return "nurbs surface";
		case FbxNodeAttribute::eShape:
			return "shape";
		case FbxNodeAttribute::eLODGroup:
			return "lodgroup";
		case FbxNodeAttribute::eSubDiv:
			return "subdiv";
		default:
			return "unknown";
		}
	};

	inline void PrintNodeAttribute(FbxNodeAttribute* InNodeAttribute)
	{
		if (!InNodeAttribute)
		{
			return;
		}

		FbxString typeName = GetAttributeTypeName(InNodeAttribute->GetAttributeType());
		JText     attrName = InNodeAttribute->GetName();

		LOG_CORE_INFO("Attribute: {}\n", typeName.Buffer());
		LOG_CORE_INFO("Attribute Name: {}\n", attrName);
	};

	inline void PrintNode(FbxNode* InNode)
	{
		const char* nodeName    = InNode->GetName();
		FbxDouble3  translation = InNode->LclTranslation.Get();
		FbxDouble3  rotation    = InNode->LclRotation.Get();
		FbxDouble3  scaling     = InNode->LclScaling.Get();

		LOG_CORE_INFO("Position: {0}, {1}, {2}\n", translation[0], translation[1], translation[2]);
		LOG_CORE_INFO("Rotation: {0}, {1}, {2}\n", rotation[0], rotation[1], rotation[2]);
		LOG_CORE_INFO("Scaling: {0}, {1}, {2}\n", scaling[0], scaling[1], scaling[2]);

		for (int i = 0; i < InNode->GetNodeAttributeCount(); i++)
		{
			PrintNodeAttribute(InNode->GetNodeAttributeByIndex(i));
		}

		for (int i = 0; i < InNode->GetChildCount(); i++)
		{
			PrintNode(InNode->GetChild(i));
		}
	}

	[[nodiscard]] inline FbxVector4 ReadNormal(const FbxMesh*         InMesh, int32_t VertexNormalCount,
											   FbxLayerElementNormal* VertexNormalSets,
											   int32_t                ControlPointIndex, int32_t VertexIndex)
	{
		FbxVector4 resultNormal(0, 0, 0);

		if (VertexNormalCount < 1)
			return resultNormal;

		const bool bMappingModeControlPoint = VertexNormalSets->GetMappingMode() == FbxGeometryElement::eByControlPoint;

		int32_t index = bMappingModeControlPoint ? ControlPointIndex : VertexIndex;

		switch (VertexNormalSets->GetReferenceMode())
		{
		case FbxLayerElement::eDirect: // Control Points to 1(uv coord) 
			resultNormal[0] = static_cast<float>(
				VertexNormalSets->GetDirectArray().GetAt(index).mData[0]);
			resultNormal[1] = static_cast<float>(
				VertexNormalSets->GetDirectArray().GetAt(index).mData[1]);
			resultNormal[2] = static_cast<float>(
				VertexNormalSets->GetDirectArray().GetAt(index).mData[2]);
			break;
		case FbxLayerElement::eIndexToDirect: // Vertex to 1(uv coord)
			index = VertexNormalSets->GetIndexArray().GetAt(index);
			resultNormal[0] = static_cast<float>(
				VertexNormalSets->GetDirectArray().GetAt(index).mData[0]);
			resultNormal[1] = static_cast<float>(
				VertexNormalSets->GetDirectArray().GetAt(index).mData[1]);
			resultNormal[2] = static_cast<float>(
				VertexNormalSets->GetDirectArray().GetAt(index).mData[2]);
			break;
		default:
			return resultNormal;
		}


		return resultNormal;
	}

	[[nodiscard]] inline FbxVector4 ReadTangent(const FbxMesh*          InMesh, int32_t VertexNormalCount,
												FbxLayerElementTangent* VertexTangentSets,
												int32_t                 ControlPointIndex, int32_t VertexIndex)
	{
		FbxVector4 resultNormal(0, 0, 0);

		if (VertexNormalCount < 1)
			return resultNormal;

		const bool bMappingModeControlPoint = VertexTangentSets->GetMappingMode() == FbxGeometryElement::eByControlPoint;

		int32_t index = bMappingModeControlPoint ? ControlPointIndex : VertexIndex;

		switch (VertexTangentSets->GetReferenceMode())
		{
		case FbxLayerElement::eDirect: // Control Points to 1(uv coord) 
			resultNormal[0] = static_cast<float>(
				VertexTangentSets->GetDirectArray().GetAt(index).mData[0]);
			resultNormal[1] = static_cast<float>(
				VertexTangentSets->GetDirectArray().GetAt(index).mData[1]);
			resultNormal[2] = static_cast<float>(
				VertexTangentSets->GetDirectArray().GetAt(index).mData[2]);
			break;
		case FbxLayerElement::eIndexToDirect: // Vertex to 1(uv coord)
			index = VertexTangentSets->GetIndexArray().GetAt(index);
			resultNormal[0] = static_cast<float>(
				VertexTangentSets->GetDirectArray().GetAt(index).mData[0]);
			resultNormal[1] = static_cast<float>(
				VertexTangentSets->GetDirectArray().GetAt(index).mData[1]);
			resultNormal[2] = static_cast<float>(
				VertexTangentSets->GetDirectArray().GetAt(index).mData[2]);
			break;
		default:
			return resultNormal;
		}


		return resultNormal;
	}

	[[nodiscard]] inline FbxVector4 ReadBinormal(const FbxMesh*           InMesh, int32_t VertexNormalCount,
												 FbxLayerElementBinormal* VertexBinormalSets,
												 int32_t                  ControlPointIndex, int32_t VertexIndex)
	{
		FbxVector4 resultNormal(0, 0, 0);

		if (VertexNormalCount < 1)
			return resultNormal;

		const bool bMappingModeControlPoint = VertexBinormalSets->GetMappingMode() == FbxGeometryElement::eByControlPoint;

		int32_t index = bMappingModeControlPoint ? ControlPointIndex : VertexIndex;

		switch (VertexBinormalSets->GetReferenceMode())
		{
		case FbxLayerElement::eDirect: // Control Points to 1(uv coord) 
			resultNormal[0] = static_cast<float>(
				VertexBinormalSets->GetDirectArray().GetAt(index).mData[0]);
			resultNormal[1] = static_cast<float>(
				VertexBinormalSets->GetDirectArray().GetAt(index).mData[1]);
			resultNormal[2] = static_cast<float>(
				VertexBinormalSets->GetDirectArray().GetAt(index).mData[2]);
			break;
		case FbxLayerElement::eIndexToDirect: // Vertex to 1(uv coord)
			index = VertexBinormalSets->GetIndexArray().GetAt(index);
			resultNormal[0] = static_cast<float>(
				VertexBinormalSets->GetDirectArray().GetAt(index).mData[0]);
			resultNormal[1] = static_cast<float>(
				VertexBinormalSets->GetDirectArray().GetAt(index).mData[1]);
			resultNormal[2] = static_cast<float>(
				VertexBinormalSets->GetDirectArray().GetAt(index).mData[2]);
			break;
		default:
			return resultNormal;
		}


		return resultNormal;
	}

	[[nodiscard]] inline FbxVector2 ReadUV(const FbxMesh*     InMesh, int32_t VertexTextureCount,
										   FbxLayerElementUV* VertexTextureSets,
										   int32_t            UVIndex, int32_t VertexIndex)
	{
		FbxVector2 resultUV(0, 0);

		if (VertexTextureCount < 1 || !VertexTextureSets)
			return resultUV;

		const bool bDirectMode = VertexTextureSets->GetDirectArray().GetAt(VertexIndex);
		FbxVector2 fbxUV;

		switch (VertexTextureSets->GetMappingMode())
		{
		case FbxLayerElementUV::eByControlPoint:
			switch (VertexTextureSets->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
				fbxUV = VertexTextureSets->GetDirectArray().GetAt(VertexIndex);
				resultUV.mData[0] = fbxUV.mData[0];
				resultUV.mData[1] = fbxUV.mData[1];
				break;
			case FbxLayerElementUV::eIndexToDirect:
				fbxUV = VertexTextureSets->GetDirectArray().GetAt(VertexTextureSets->GetIndexArray().GetAt(VertexIndex));
				resultUV.mData[0] = fbxUV.mData[0];
				resultUV.mData[1] = fbxUV.mData[1];
				break;
			case FbxLayerElement::eIndex:
				break;
			}
			break;
		case FbxLayerElementUV::eByPolygonVertex:
			resultUV.mData[0] = VertexTextureSets->GetDirectArray().GetAt(UVIndex).mData[0];
			resultUV.mData[1] = VertexTextureSets->GetDirectArray().GetAt(UVIndex).mData[1];
			break;
		default:
			return resultUV;
		}

		return resultUV;
	}

	[[nodiscard]] inline FbxColor ReadColor(const FbxMesh*              InMesh, int32_t VertexColorCount,
											FbxLayerElementVertexColor* VertexColorSets,
											int32_t                     ColorIndex, int32_t VertexIndex)
	{
		FbxColor returnColor(1, 1, 1, 1);
		if (VertexColorCount < 1 || !VertexColorSets)
			return returnColor;

		const int32_t                        vertexColorCount = InMesh->GetElementVertexColorCount();
		const FbxGeometryElementVertexColor* vertexColor      = InMesh->GetElementVertexColor(0);

		int32_t index = -1;

		switch (VertexColorSets->GetMappingMode())
		{
		case FbxLayerElement::eByControlPoint:
			switch (VertexColorSets->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
				return VertexColorSets->GetDirectArray().GetAt(ColorIndex);
			case FbxLayerElement::eIndexToDirect:
				index = VertexColorSets->GetIndexArray().GetAt(ColorIndex);
				break;
			default:
				return returnColor;
			}
		case FbxLayerElement::eByPolygonVertex:
			switch (VertexColorSets->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
				index = VertexIndex;
				break;
			case FbxLayerElement::eIndexToDirect:
				index = VertexColorSets->GetIndexArray().GetAt(VertexIndex);
				break;
			default:
				return returnColor;
			}
			break;
		default:
			return returnColor;
		}

		return VertexColorSets->GetDirectArray().GetAt(index);
	}

#pragma endregion

	inline void ParseMaterialProps(FbxProperty&       Property,
								   const char*        ParamName,
								   JMaterialInstance* Material)
	{
		/// 텍스처가 일반 텍스처인지 레이어드 텍스처인지 확인
		/// 레이어드 텍스처?
		///  - 여러개의 텍스처를 하나로 합쳐서 사용하는 방식
		///  - 더 복잡한 방식으로 텍스처들이 혼합되었기 때문에 여러 텍스처가 있을 수 있음
		FMaterialParam materialParams;
		{
			materialParams.Name         = ParamName;
			materialParams.TextureValue = nullptr;
		}

		const int32_t layeredTextureCount = Property.GetSrcObjectCount<FbxLayeredTexture>();
		const int32_t textureCount        = Property.GetSrcObjectCount<FbxTexture>();

		// 텍스처 레이어가 여러개 일 경우
		if (layeredTextureCount > 0)
		{
			int32_t textureIndex = 0;

			for (int32_t i = 0; i < layeredTextureCount; ++i)
			{
				FbxLayeredTexture* fbxLayeredTexture = Property.GetSrcObject<FbxLayeredTexture>(i);
				int32_t            layeredTextureNum = fbxLayeredTexture->GetSrcObjectCount<FbxTexture>();

				for (int32_t j = 0; j < layeredTextureNum; ++j)
				{
					if (Property.GetSrcObject<FbxTexture>(j))
					{
						const FbxFileTexture* fileTexture = Property.GetSrcObject<FbxFileTexture>(j);
						materialParams                    = Material::CreateTextureParam(
																	  ParamName,
																	  fileTexture->GetFileName(),
																	  textureIndex++);
					}
				}
			}
		}

		// 텍스처 레이어가 없고 단일 텍스처일 경우
		else if (textureCount > 0)
		{
			for (int32_t i = 0; i < textureCount; ++i)
			{
				if (Property.GetSrcObject<FbxTexture>(i))
				{
					const FbxFileTexture* fileTexture = Property.GetSrcObject<FbxFileTexture>(i);
					materialParams                    = Material::CreateTextureParam(ParamName,
																  fileTexture->GetFileName(),
																  i);
				}
			}
		}

		// 텍스처가 없을 경우 숫자값을 파싱
		else
		{
			switch (Property.GetPropertyDataType().GetType())
			{
			case eFbxBool:
				materialParams.ParamValue = EMaterialParamValue::Boolean;
				materialParams.BooleanValue = Property.Get<FbxBool>();
				break;
			case eFbxInt:
				materialParams.ParamValue = EMaterialParamValue::Integer;
				materialParams.IntegerValue = Property.Get<FbxInt>();
				break;
			case eFbxFloat:
				materialParams.ParamValue = EMaterialParamValue::Float;
				materialParams.FloatValue = Property.Get<FbxFloat>();
				break;
			case eFbxDouble:
				materialParams.ParamValue = EMaterialParamValue::Float;
				materialParams.FloatValue = Property.Get<FbxDouble>();
				break;
			case eFbxDouble2:
				materialParams.ParamValue = EMaterialParamValue::Float2;
				materialParams.Float2Value.x = Property.Get<FbxDouble2>().mData[0];
				materialParams.Float2Value.y = Property.Get<FbxDouble2>().mData[1];
				break;
			case eFbxDouble3:
				materialParams.ParamValue = EMaterialParamValue::Float3;
				materialParams.Float3Value.x = Property.Get<FbxDouble3>().mData[0];
				materialParams.Float3Value.y = Property.Get<FbxDouble3>().mData[1];
				materialParams.Float3Value.z = Property.Get<FbxDouble3>().mData[2];
				break;
			case eFbxDouble4:
				materialParams.ParamValue = EMaterialParamValue::Float4;
				materialParams.Float4Value.x = Property.Get<FbxDouble4>().mData[0];
				materialParams.Float4Value.y = Property.Get<FbxDouble4>().mData[1];
				materialParams.Float4Value.z = Property.Get<FbxDouble4>().mData[2];
				materialParams.Float4Value.w = Property.Get<FbxDouble4>().mData[3];
				break;
			default:
				LOG_CORE_TRACE("Unknown Property Type");
				break;
			}
		}

		Material->EditInstanceParam(ParamName, materialParams);
	}

	[[nodiscard]] inline FMatrix ParseTransform(FbxNode* InNode, const FMatrix& ParentWorldMat)
	{
		FbxVector4 translation;
		FbxVector4 rotation;
		FbxVector4 scale;

		// lcl(Transform...).Get()은 부모로 부터의 상대적인 Transform을 반환
		if (InNode->LclTranslation.IsValid())
		{
			translation = InNode->LclTranslation.Get();
		}
		if (InNode->LclRotation.IsValid())
		{
			rotation = InNode->LclRotation.Get();
		}
		if (InNode->LclScaling.IsValid())
		{
			scale = InNode->LclScaling.Get();
		}

		FbxMatrix transform(translation, rotation, scale);
		FMatrix   local = Maya2DXMat(FMat2JMat(transform));
		FMatrix   world = local * ParentWorldMat;

		return world;
	}

	[[nodiscard]] inline JMaterialInstance* ParseLayerMaterial(const FbxMesh* InMesh, const int32_t InMaterialIndex,
															   bool&          bShouldSerialize)
	{
		FbxSurfaceMaterial* fbxMaterial = InMesh->GetNode()->GetMaterial(InMaterialIndex);
		assert(fbxMaterial);

		JText fileName = std::format("Game/Materials/{0}/{1}.jasset", InMesh->GetName(), fbxMaterial->GetName());
		/** 셰이딩 모델은 거의 PhongShading */

		JMaterialInstance* matInstance = MMaterialInstanceManager::Get().CreateOrLoad(fileName);
		// 머티리얼이 이미 존재할 경우 아래 파싱 과정 생략
		if (Serialization::IsJAssetFileAndExist(fileName.c_str()))
		{
			bShouldSerialize = false;
			return matInstance;
		}

		matInstance->SetAsDefaultMaterial();

		/// 머티리얼 파싱
		/// FBX SDK에서 정확히 Property Name이 어떻게 설정되어있는지 모르겠다.
		/// Metallic에 관련된 텍스처를 뽑고 싶어도 FBX SDK에서는 Metallic이라는 이름을 찾을 수 없다.
		/// (Metallic뿐 아니라 PBR(Pyshically Based Rendering)에 관련된 텍스처이름들을 찾을 수 없다.)
		/// 수동으로 모든 프로퍼티들을 파싱해보니 다른 프로퍼티로 저장되어 있을 수 있음
		/// 따라서 머티리얼 내에 존재하는 모든 텍스처(우리에게 필수적인)부터 파싱하고, 없을 경우 Color값을 파싱한다.

		/** 현재 레이어의 머티리얼에서 추출하고자 하는 텍스처 타입(FbxSurfaceMaterial)이 존재할 경우 추출 */
		for (int32_t i = 0; i < ARRAYSIZE(FbxMaterialProperties); ++i)
		{
			const FFbxProperty& textureParams = FbxMaterialProperties[i];

			// FindProperty가 내부적으로 어떤 알고리듬을 사용하는지 모르겠다...
			// O(1)보다 크다면 그냥 순차적으로 하나씩 돌리는게 나음 (큰 차이는 없음)
			FbxProperty property = fbxMaterial->FindProperty(textureParams.FbxPropertyName);
			if (property.IsValid())
			{
				ParseMaterialProps(property,
								   textureParams.PropertyName,
								   matInstance);
			}
		}

		bShouldSerialize = true;


		return matInstance;
	}


}
