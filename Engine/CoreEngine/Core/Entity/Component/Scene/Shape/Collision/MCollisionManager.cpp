﻿#include "MCollisionManager.h"

#include "Core/Interface/JWorld.h"
#include "Game/Src/Player/APlayerCharacter.h"

void MCollisionManager::EnrollCollision(ICollision* InCollision)
{
	mLayerHash[InCollision->GetTraceType()].emplace_back(InCollision);
}

void MCollisionManager::UnEnrollCollision(ICollision* InCollision)
{
	const ETraceType traceType = InCollision->GetTraceType();
	if (mLayerHash.contains(traceType))
		std::erase(mLayerHash[traceType], InCollision);
}

void MCollisionManager::UnEnrollAllCollision()
{
	for (auto& layer : mLayerHash)
	{
		layer.second.clear();
	}
}

void MCollisionManager::SetCollisionLayer(ETraceType InLeft, ETraceType InRight, bool bEnable)
{
	const uint8_t leftLayer  = EnumAsByte(InLeft);
	const uint8_t rightLayer = EnumAsByte(InRight);

	mCollisionLayer[leftLayer][rightLayer] = bEnable;
}

JArray<ICollision*>& MCollisionManager::GetLayer(ETraceType InType)
{
	return mLayerHash[InType];
}

void MCollisionManager::UpdateCollision()
{
	// auto* player = MLevelManager::Get().GetActiveLevel()->mPlayerCharacter;
	// auto& pawns  = MLevelManager::Get().GetActiveLevel()->Pawns;
	//
	// const JArray<ICollision*>& blockingVolume = mLayerHash[ETraceType::BlockingVolume];
	// const JArray<ICollision*>& interactions   = mLayerHash[ETraceType::Interactive];
	// const JArray<>
	//
	// for (auto& volume : blockingVolume)
	// {
	// 	for (auto& pawn : pawns)
	// 	{
	// 		CheckCollision(volume, pawn->PawnSphere());
	// 	}
	// }
	//
	// for (auto& pawn : pawns)
	// {
	// 	for (auto& pawn2 : pawns)
	// 	{
	// 		if (pawn != pawn2)
	// 		{
	// 			CheckCollision(pawn->PawnSphere(), pawn2->PawnSphere());
	// 		}
	// 	}
	// }
	//
	// for (auto& interaction : interactions)
	// {
	// 	CheckCollision(player->PawnSphere(), interaction);
	// }
	//
	// for (auto& pawn : pawns)
	// {
	// 	CheckCollision(pawn->PawnSphere(), player->WeaponCollider());
	// }


	// 2차원 배열(각 레이어별로)을 순회하면서 충돌을 검사한다.
	for (uint8_t i = 0; i < EnumAsByte(ETraceType::Max); ++i)
	{
		for (uint8_t j = 0; j < EnumAsByte(ETraceType::Max); ++j)
		{
			// 충돌검사를 하도록 설정된 레이어만 검사한다.
			if (mCollisionLayer[i][j])
			{
				UpdateLayerCollision(static_cast<ETraceType>(i), static_cast<ETraceType>(j));
			}
		}
	}


}

void MCollisionManager::UpdateLayerCollision(ETraceType InRow, ETraceType InCol)
{
	// 내부에 존재하는 오브젝트 리스트를 가져온다.
	const JArray<ICollision*>& row = mLayerHash[InRow];
	const JArray<ICollision*>& col = mLayerHash[InCol];

	for (auto& rowCollision : row)
	{
		for (auto& colCollision : col)
		{
			// 동일한 오브젝트(자기자신)는 충돌검사를 하지 않는다.
			if (rowCollision != colCollision)
			{
				CheckCollision(rowCollision, colCollision);
			}
		}
	}
}

void MCollisionManager::CheckCollision(ICollision* InLeft, ICollision* InRight)
{
	CollisionID id(InLeft->GetCollisionID(), InRight->GetCollisionID());

	// 부모 자식간의 충돌검사는 하지 않는다.
	if ((InLeft->GetActorID() == InRight->GetActorID()) || !(InLeft->CollisionEnabled() && InRight->CollisionEnabled()))
	{
		return;
	}

	if (InLeft->GetNodeIndex() != InRight->GetNodeIndex() && !(InLeft->GetNodeIndex() == 0 || InRight->GetNodeIndex() ==
		0))
	{
		return;
	}

	if (!mCollisionHash.contains(id.ID))
	{
		mCollisionHash[id.ID] = false;
	}

	FHitResult hitResult{};
	hitResult.SrcCollision = InLeft;
	hitResult.DstCollision = InRight;

	if (Intersect(InLeft, InRight, hitResult))
	{
		if (mCollisionHash[id.ID])
		{
			InLeft->OnComponentOverlap.Execute(InRight, hitResult);
			InRight->OnComponentOverlap.Execute(InLeft, hitResult);
		}
		else
		{
			InLeft->OnComponentBeginOverlap.Execute(InRight, hitResult);
			InRight->OnComponentBeginOverlap.Execute(InLeft, hitResult);

			mCollisionHash[id.ID] = true;
		}
	}
	else
	{
		if (mCollisionHash[id.ID])
		{
			InLeft->OnComponentEndOverlap.Execute(InRight, hitResult);
			InRight->OnComponentEndOverlap.Execute(InLeft, hitResult);

			mCollisionHash[id.ID] = false;
		}
	}

}

bool MCollisionManager::Intersect(ICollision* InLeft, ICollision* InRight, FHitResult& OutHitResult)
{
	if (InLeft && InRight)
	{
		return InLeft->Intersect(InRight, OutHitResult);
	}

	return false;
}

MCollisionManager::MCollisionManager() {}
MCollisionManager::~MCollisionManager() {}
