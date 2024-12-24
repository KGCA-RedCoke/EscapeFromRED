#pragma once
#include <bitset>

#include "Core/Entity/Component/Scene/Shape/JShape.h"
#include "Core/Manager/Manager_Base.h"

union CollisionID
{
	struct
	{
		uint32_t Left;
		uint32_t Right;
	};

	uint64_t ID;
};

class MCollisionManager : public TSingleton<MCollisionManager>
{

public:
	void EnrollCollision(ICollision* InCollision);
	void UnEnrollCollision(ICollision* InCollision);
	void SetCollisionLayer(ETraceType InLeft, ETraceType InRight, bool bEnable);
	void UpdateCollision();

private:
	void UpdateLayerCollision(ETraceType InRow, ETraceType InCol);
	void CheckCollision(ICollision* InLeft, ICollision* InRight);
	bool Intersect(ICollision* InLeft, ICollision* InRight);

private:
	JHash<ETraceType, JArray<ICollision*>>   mLayerHash;
	JHash<uint64_t, bool>                    mCollisionHash;
	std::bitset<EnumAsByte(ETraceType::Max)> mCollisionLayer[EnumAsByte(ETraceType::Max)];

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MCollisionManager>;
	MCollisionManager();
	~MCollisionManager();

public:
	MCollisionManager(const MCollisionManager&)            = delete;
	MCollisionManager& operator=(const MCollisionManager&) = delete;
#pragma endregion
};
