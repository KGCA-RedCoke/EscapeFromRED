#include "SpaceDivision.h"

void Oc::FNode::Subdivide()
{
	const FVector center = BoundArea.Center;
	const FVector extent = BoundArea.Extent * 0.5f;	// 부모 노드의 크기의 절반
	const FVector min    = center - extent;
	const FVector max    = center + extent;

	// 8개 자식 노드의 중심점을 계산
	const FVector childCenters[8] = {
		FVector(min.x, min.y, min.z), // 앞, 왼쪽, 아래
		FVector(max.x, min.y, min.z), // 앞, 오른쪽, 아래
		FVector(min.x, max.y, min.z), // 뒤, 왼쪽, 아래
		FVector(max.x, max.y, min.z), // 뒤, 오른쪽, 아래
		FVector(min.x, min.y, max.z), // 앞, 왼쪽, 위
		FVector(max.x, min.y, max.z), // 앞, 오른쪽, 위
		FVector(min.x, max.y, max.z), // 뒤, 왼쪽, 위
		FVector(max.x, max.x, max.z)  // 뒤, 오른쪽, 위
	};

	// 각 자식 노드 생성
	for (int i = 0; i < 8; ++i)
	{
		Children[i]         = MakeUPtr<FNode>();
		Children[i]->Parent = this;
		Children[i]->Depth  = Depth + 1;

		// 자식 노드의 BoundArea 설정
		Children[i]->BoundArea = FBox(childCenters[i], extent);
	}

	// 현재 노드는 더 이상 리프 노드가 아님
	bIsLeaf = false;
}

void Oc::JTree::Initialize(const FBox& InRootBoundArea, uint32_t InDepth)
{
	if (mRootNode)
	{
		return;
	}

	mRootNode            = MakeUPtr<FNode>();
	mRootNode->BoundArea = InRootBoundArea;

	Subdivide(mRootNode.get(), InDepth);
}

void Oc::JTree::Subdivide(FNode* InNode, uint32_t InDepth)
{
	if (InNode->Depth >= InDepth || !InNode) // 최대 깊이에 도달하거나 잘못된 노드일 경우 종료
	{
		return;
	}

	// 노드 분할
	InNode->Subdivide();

	// 각 자식 노드에 대해 재귀적으로 분할 수행
	for (int i = 0; i < 8; ++i)
	{
		InNode->Children[i]->Depth = InNode->Depth + 1; // 자식 노드의 깊이 설정
		Subdivide(InNode->Children[i].get(), InDepth);
	}
}
