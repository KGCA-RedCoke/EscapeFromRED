#pragma once
#include "common_include.h"
#include "Core/Entity/Component/Scene/Shape/JShape.h"

namespace Oc
{
	struct FNode
	{
		uint32_t Depth          = 0;
		uint32_t CornerIndex[4] = {0, 0, 0, 0};
		bool     bIsLeaf        = false;

		FBox BoundArea;

		JArray<class AActor*> Actors;

		FNode*      Parent      = nullptr;
		UPtr<FNode> Children[8] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

		void SetBoundArea(const FBox& InBoundArea) { BoundArea = InBoundArea; }
		void SetBoundArea(const FVector& Center, const FVector& Extent) { BoundArea = FBox(Center, Extent); }
		void Subdivide();
	};

	class JTree
	{
	public:
		JTree() = default;

	public:
		void Initialize(const FBox& InRootBoundArea, uint32_t InDepth);
		void Update();
		void Insert(AActor* InActor);

	private:
		void Subdivide(FNode* InNode, uint32_t InDepth);

	private:
		UPtr<FNode> mRootNode = nullptr;

	};
}
