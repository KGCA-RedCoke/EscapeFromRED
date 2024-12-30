#pragma once
#include "common_include.h"
#include "Core/Entity/Component/Scene/Shape/JShape.h"

class AActor;
class JCameraComponent;

#define MAX_DEPTH 4

namespace Quad
{

	struct FNode
	{
		uint32_t Index          = 0;
		uint32_t Depth          = 0;
		uint32_t CornerIndex[4] = {0, 0, 0, 0};
		bool     bIsLeaf        = false;

		FQuad     BoundArea;
		FBoxShape BoundBox;

		JArray<AActor*> Actors;

		FNode*      Root        = nullptr;
		FNode*      Parent      = nullptr;
		UPtr<FNode> Children[4] = {nullptr, nullptr, nullptr, nullptr};

		void Update();
		void Render(JCameraComponent* InCamera); 

		void Subdivide(FNode* InRoot);
		void Insert(AActor* InActor);
		void InsertIntoChildren(AActor* InActor);
		bool Remove(AActor* InActor);
		void Clear();
	};

	class JTree
	{
	public:
		JTree() = default;

	public:
		void Initialize(const FQuad& InRootBoundArea, uint32_t InDepth);
		void Update();
		void Insert(AActor* InActor);
		void Render(JCameraComponent* InCamera);
		void Remove(AActor* InActor);
		void Sort(AActor* InActor);

	private:
		void Subdivide(FNode* InNode, uint32_t InDepth, FNode* InRoot);

	public:
		JArray<AActor*> IgnoreFrustumActors;

	private:
		UPtr<FNode> mRootNode = nullptr;

	};
}
