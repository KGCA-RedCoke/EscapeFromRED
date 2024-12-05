#include "SpaceDivision.h"
#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Camera/JCameraComponent.h"

void Oc::FNode::Render(JCameraComponent* InCamera) const
{
	if (!Parent || InCamera->IsBoxInFrustum(BoundArea))
	{
		for (const auto& actor : Actors)
		{
			actor->Draw();
		}
		for (int i = 0; i < 8; ++i)
		{
			if (Children[i])
			{
				Children[i]->Render(InCamera);
			}
		}
	}

}

void Oc::FNode::Subdivide()
{
	const FVector center = BoundArea.Box.Center;
	const FVector extent = BoundArea.Box.Extent * 0.5f;	// 부모 노드의 크기의 절반
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
		Children[i]->BoundArea = FBoxShape(childCenters[i], extent);
	}

	// 현재 노드는 더 이상 리프 노드가 아님
	bIsLeaf = false;
}

void Oc::FNode::Insert(AActor* InActor)
{
	// 현재 노드가 리프 노드이고 액터를 직접 저장할 수 있는 경우
	if (bIsLeaf)
	{
		// 최대 깊이 또는 자식 노드로 더 이상 분리할 수 없는 경우, 직접 저장
		if (Depth >= MAX_DEPTH || Children[0] != nullptr)
		{
			Actors.emplace_back(InActor);

			return;
		}

		// // 자식 노드로 분리
		// Subdivide();

		// // 기존에 저장된 액터들을 재분배
		// for (const auto& existingActor : Actors)
		// {
		// 	InsertIntoChildren(existingActor);
		// }
		// Actors.clear();
	}

	// 자식 노드 중 하나에 삽입
	InsertIntoChildren(InActor);
}

void Oc::FNode::InsertIntoChildren(AActor* InActor)
{
	const FBoxShape& actorBounds = InActor->GetBoundingVolume();

	for (int i = 0; i < 8; ++i)
	{
		if (Children[i]->BoundArea.Intersect(actorBounds))
		{
			Children[i]->Insert(InActor);
			return;
		}
	}
	// 액터가 어떤 자식 노드와도 교차하지 않으면, 현재 노드에 저장
	Actors.emplace_back(InActor);
}

void Oc::JTree::Initialize(const FBoxShape& InRootBoundArea, uint32_t InDepth)
{
	if (mRootNode)
	{
		return;
	}

	mRootNode            = MakeUPtr<FNode>();
	mRootNode->BoundArea = InRootBoundArea;

	Subdivide(mRootNode.get(), InDepth);
}

void Oc::JTree::Update()
{
	// 지워질 액터들을 찾아서 제거
	FNode* node = mRootNode.get();
	while (node)
	{
		for (int i = 0; i < 8; ++i)
		{
			if (FNode* childNode = node->Children[i].get())
			{
				childNode->Actors.erase(
										std::ranges::remove_if(
															   childNode->Actors,
															   [](AActor* actor){
																   return actor->IsPendingKill();
															   }).begin(),
										childNode->Actors.end());
			}
		}

		node = node->Children[0].get();
	}
}

void Oc::JTree::Insert(AActor* InActor)
{
	if (InActor->IsIgnoreFrustum())
	{
		IgnoreFrustumActors.emplace_back(InActor);
		return;
	}
	mRootNode->Insert(InActor);
}

void Oc::JTree::Render(JCameraComponent* InCamera)
{
	for (const auto& actor : IgnoreFrustumActors)
	{
		actor->Draw();
	}

	// 모든 노드 순회하면서 프러스텀내에 있는 액터들 렌더링
	mRootNode->Render(InCamera);
}

void Oc::JTree::Subdivide(FNode* InNode, uint32_t InDepth)
{
	if (!InNode)
	{
		return;
	}
	if (InNode->Depth >= InDepth) // 최대 깊이에 도달하거나 잘못된 노드일 경우 종료
	{
		InNode->bIsLeaf = true;
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
