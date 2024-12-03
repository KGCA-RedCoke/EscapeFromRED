#pragma once
#include "common_include.h"
#include "Core/Entity/Component/Scene/Shape/JShape.h"

class AActor;
class JCameraComponent;

#define MAX_DEPTH 3

namespace Oc
{

	struct FNode
	{
		uint32_t Depth          = 0;
		uint32_t CornerIndex[4] = {0, 0, 0, 0};
		bool     bIsLeaf        = false;

		FBoxShape BoundArea;

		JArray<AActor*> Actors;

		FNode*      Parent      = nullptr;
		UPtr<FNode> Children[8] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

		void Render(JCameraComponent* InCamera) const;

		void SetBoundArea(const FBoxShape& InBoundArea) { BoundArea = InBoundArea; }
		void Subdivide();
		void Insert(AActor* InActor);
		void InsertIntoChildren(AActor* InActor);
	};

	class JTree
	{
	public:
		JTree() = default;

	public:
		void Initialize(const FBoxShape& InRootBoundArea, uint32_t InDepth);
		void Update();
		void Insert(AActor* InActor);
		void Render(JCameraComponent* InCamera);

	private:
		void Subdivide(FNode* InNode, uint32_t InDepth);

	public:
		JArray<AActor*> IgnoreFrustumActors;

	private:
		UPtr<FNode> mRootNode = nullptr;

	};
}
