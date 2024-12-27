#include "MCollisionManager.h"

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

void MCollisionManager::SetCollisionLayer(ETraceType InLeft, ETraceType InRight, bool bEnable)
{
	const uint8_t leftLayer  = EnumAsByte(InLeft);
	const uint8_t rightLayer = EnumAsByte(InRight);

	mCollisionLayer[leftLayer][rightLayer] = bEnable;
}

void MCollisionManager::UpdateCollision()
{
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
	CollisionID id;
	id.Left  = InLeft->GetCollisionID();
	id.Right = InRight->GetCollisionID();

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
			InLeft->OnComponentBeginOverlap.Execute(InRight,hitResult);
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
		FHitResult hitResult;
		return InLeft->Intersect(InRight, OutHitResult);
	}

	return false;
}

MCollisionManager::MCollisionManager() {}
MCollisionManager::~MCollisionManager() {}
