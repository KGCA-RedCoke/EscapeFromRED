#pragma once

#include <fbxsdk.h>
#include "FbxUtils.h"
#include "Core/Utils/Math/TMatrix.h"

class JSkinData;
class JMeshData;
class JMaterial;

// Fbx 파일 구조에 대해 자세하게 나와있는 링크
// https://ics.uci.edu/~djp3/classes/2014_03_ICS163/tasks/arMarker/Unity/arMarker/Assets/CactusPack/Meshes/Sprites/Rock_Medium_SPR.fbx
// https://banexdevblog.wordpress.com/2014/06/23/a-quick-tutorial-about-the-fbx-ascii-format/
// (Parsing tutorial)
// GameDev : https://www.gamedev.net/tutorials/_/technical/graphics-programming-and-theory/how-to-work-with-fbx-sdk-r3582/
namespace Utils::Fbx
{
	/**
	* FBX 파일을 로드하고 파싱하는 클래스
	*/
	class FbxFile
	{
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
		/**
		 * 실질적인 Fbx 파일 로드
		 * @param InFilePath 파일 경로
		 */
		bool Load(const char* InFilePath);
		
		/**
		 * FBX 파일을 파싱하여 Raw 데이터로 변환
		 * FIXME: 인덱스 버퍼를 생성하는 과정에서 시간이 많이 소요됨 최적화 필요
		 */
		bool Convert();

	private:
		/** 내부적으로 Parsing함수들을 실행 */
		void ProcessLoad();

		/// ---------------------------- Step1. 스켈레톤 정보를 파싱 ----------------------------
		/// 이 과정은 가장 먼저 선행된다. 스켈레톤 정보가 있으면 추후 메시 파싱시 스킨 정보를 파싱할 수 있음
		/// 또 우리가 사용할 애니메이션은 모두 캐릭터(본을 가지는 객체)이므로 스켈레톤 정보가 필수적이다.
		/// 따라서 스켈레톤이 없으면 스킨과 애니메이션을 파싱 할 필요가 없어짐

		/**
		 * @brief 스켈레톤을 파싱하는 함수
		 * 전체적인 파싱 과정중 첫번째 과정
		 * 스켈레톤(본) 정보가 있으면 추후 메시 파싱시 스킨 정보를 파싱할 수 있음
		 * @param InNode 파싱 노드
		 * @param InIndex 본 인덱스
		 * @param InParentIndex 부모 본 인덱스
		 */
		void ParseSkeleton_Recursive(FbxNode* InNode, int32_t InIndex, int32_t InParentIndex);


		/// ---------------------------- Step2. 메시 정보를 파싱 ----------------------------
		/// 메시가 있다면 메시정보를 파싱한다.
		/// Case 1. 스켈레톤 정보가 없다면 메시 정보만 파싱한다.
		/// Case 2. 스켈레톤 정보가 있다면 스킨 정보도 파싱한다.
		/// Case 3. 메시가 없다면 아무것도 하지 않는다.

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

		/**
		 * @brief 메시노드의 속성을 파싱, 특히 스킨을 파싱하는 함수
		 * @param InMesh 메시노드 
		 * @param InSkinData 메시 스킨 데이터
		 */
		void ParseMeshSkin(const FbxMesh* InMesh, JSkinData* InSkinData);

		/// ---------------------------- Step3. 애니메이션 정보를 파싱 ----------------------------
		/// Check List
		///  스켈레톤 정보가 있나?
		/// 스켈레톤이 있을 때만 애니메이션 정보를 파싱한다.
		void ParseAnimation(FbxScene* InScene);
		
		void ParseAnimationStack(FbxScene* Scene, FbxString* AnimStackName);

		void ParseAnimNode(FbxNode* InNode, int32_t InParentIndex, bool bSkeletalAnim = false);

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

		void CaptureBindPoseMatrix(JSkinData*        Ptr, const FbxNode* Joint,
								   const FbxMatrix& InBindPosMat);

		void CaptureAnimation(const std::shared_ptr<JAnimationClip>& Ptr, FbxScene* Scene);

		void AddKey(FAnimationNode& InNode, FAnimationNode* InParentNode, const FbxAMatrix& InGlobalMat, float InTime);

	private:
		FbxImporter* mFbxImporter;
		FbxScene*    mFbxScene;

		bool          bHasSkeleton = false;
		FSkeletonData mSkeletonData;
		// Mesh를 배열로 저장하는 이유는 노드에 여러 메시가 붙어있을 수 있기 때문
		JArray<Ptr<JMeshData>> mMeshList;
		// Layer0만(거의 0에 다 들어있음) 사용할 것이므로 사실상 1개(mMaterialList[0])만 사용
		JArray<JArray<Ptr<JMaterialInstance>>> mMaterialList;


		JArray<FAnimationNode>      mScanList;
		JArray<Ptr<JAnimationClip>> mAnimations;

		int32_t mNumVertex = 0;
		int32_t mNumIndex  = 0;
	};
}
