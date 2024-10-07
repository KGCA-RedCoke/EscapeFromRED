#pragma once

#include <fbxsdk.h>
#include "FbxUtils.h"
#include "Core/Utils/Math/TMatrix.h"

class JSkeletalMesh;
class JMeshData;
class JMaterial;

// Fbx 파일 구조에 대해 자세하게 나와있는 링크
// https://ics.uci.edu/~djp3/classes/2014_03_ICS163/tasks/arMarker/Unity/arMarker/Assets/CactusPack/Meshes/Sprites/Rock_Medium_SPR.fbx
// https://banexdevblog.wordpress.com/2014/06/23/a-quick-tutorial-about-the-fbx-ascii-format/
namespace Utils::Fbx
{
	/**
	* FBX 파일을 로드하고 파싱하는 클래스
	*/
	class FbxFile
	{
		// FIXME: FbxImporter, FbxScene을 개체마다 하나씩 가져야 할 필요가 있을까?

	public:
		FbxFile() = default;
		~FbxFile();

	public:
		/** Importer, Scene 생성 */
		void Initialize(const char* InFilePath);
		/** 존재하는 모든 fbx sdk destroy (프로세스 종료 전에 호출) */
		void Release_Internal() const;

		static void Release();

	public:
		bool Load(const char* InFilePath);
		bool Convert();

	private:
		/** 내부적으로 Parsing함수들을 실행 */
		void ProcessLoad();

		/**
		 * @brief 노드를 파싱하는 함수 (재귀 호출)
		 * @param InNode 탐색할 노드
		 * @param NodeAttribute 탐색할 노드 AttributeType
		 * @param InParentMeshData
		 * @param InParentMatrix
		 */
		void ParseNode_Recursive(FbxNode*              InNode, FbxNodeAttribute::EType NodeAttribute,
								 const Ptr<JMeshData>& InParentMeshData = nullptr,
								 const FMatrix&        InParentMatrix   = FMatrix::Identity);

		/**
		 * @brief 메시노드의 속성을 파싱하는 함수
		 * @param InNode 메시노드 InNode->GetMesh()
		 * @param InMeshData 저장할 데이터 Mesh
		 */
		void ParseMesh(FbxNode* InNode, Ptr<JMeshData> InMeshData);

		void ParseSkeleton(FbxNode* InNode, Ptr<JMeshData> InMeshData);

		/**
		 * 메시의 레이어들을 분석하여 파싱한다.
		 * Fbx에서 geometry data는 layer로 표현된다.
		 * 관련 클래스들은 FbxLayerElementTemplate로 표현
		 * 보통 레이어는 1개에서 2개 정도가 일반적이다.
		 * 주로 UV, NormalMap, Tangent, Binormal, Material 등을 파싱한다.
		 * @param InMesh FbxMesh 개체
		 * @param InMeshData JMesh 개체 (파싱된 데이터를 저장할 개체)
		 */
		FLayer ParseMeshLayer(FbxMesh* InMesh, const Ptr<JMeshData>& InMeshData);

	public:
		FbxImporter* mFbxImporter;
		FbxScene*    mFbxScene;

		// Mesh를 배열로 저장하는 이유는 노드에 여러 메시가 붙어있을 수 있기 때문
		std::vector<Ptr<JMeshData>> mMeshList;
		// Layer0만(거의 0에 다 들어있음) 사용할 것이므로 사실상 1개(mMaterialList[0])만 사용
		std::vector<std::vector<Ptr<JMaterial>>> mMaterialList;

		int32_t mNumVertex = 0;
		int32_t mNumIndex  = 0;
	};
}
