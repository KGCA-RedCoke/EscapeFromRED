﻿#pragma once
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

	CollisionID(uint32_t InLeft, uint32_t InRight)
	{
		if (InLeft < InRight)
		{
			Left  = InLeft;
			Right = InRight;
		}
		else
		{
			Left  = InRight;
			Right = InLeft;
		}
	}
};

enum class ETraceType : uint8_t
{
	Pawn,
	BlockingVolume,
	Ground,
	EnemyHitSpace,
	PlayerWeapon,
	Interactive,
	Player,
	Max
};

constexpr const char* TraceTypeToString(const ETraceType InType)
{
	switch (InType)
	{
	case ETraceType::Pawn:
		return "Pawn";
	case ETraceType::BlockingVolume:
		return "BlockingVolume";
	case ETraceType::Ground:
		return "Ground";
	case ETraceType::EnemyHitSpace:
		return "EnemyHitSpace";
	case ETraceType::PlayerWeapon:
		return "PlayerWeapon";
	case ETraceType::Interactive:
		return "Interactive";
	case ETraceType::Max:
		return "Max";
	}

	return "Unknown";
}

enum class ECollisionType
{
	None,
	Quad,
	Plane,
	Ray,
	Box,
	Sphere,
	Capsule
};

class ICollision;

/**
 * 충돌 결과 구조체
 */
struct FHitResult
{
	ICollision* SrcCollision = nullptr;	// 충돌 소스 (자신)
	ICollision* DstCollision = nullptr;	// 충돌 대상 (상대)

	FVector HitLocation = FVector::ZeroVector;	// 충돌 지점
	FVector HitNormal   = FVector::ZeroVector;	// 충돌 노말
	float   Distance    = 0.f;					// 충돌 거리
};


DECLARE_DYNAMIC_DELEGATE(FOnComponentBeginOverlap, ICollision* Other, const FHitResult&);

DECLARE_DYNAMIC_DELEGATE(FOnComponentOverlap, ICollision* Other, const FHitResult&);

DECLARE_DYNAMIC_DELEGATE(FOnComponentEndOverlap, ICollision* Other, const FHitResult&);

/**
 * 충돌체 인터페이스 클래스
 */
class ICollision
{
public:
	FOnComponentBeginOverlap OnComponentBeginOverlap;
	FOnComponentOverlap      OnComponentOverlap;
	FOnComponentEndOverlap   OnComponentEndOverlap;

public:
	virtual uint32_t       GetCollisionID() = 0;
	virtual ETraceType     GetTraceType() const = 0;
	virtual ECollisionType GetCollisionType() const = 0;
	virtual FRay           GetRay() const = 0;
	virtual FBoxShape      GetBox() const = 0;
	virtual FSphere        GetSphere() const = 0;
	virtual uint32_t       GetActorID() const = 0;
	virtual uint32_t       GetNodeIndex() const = 0;
	virtual void           SetTraceType(ETraceType InType) = 0;
	virtual void           SetCollisionType(ECollisionType InType) = 0;
	virtual bool           Intersect(ICollision* InOther, _Out_ FHitResult& OutHitResult) const = 0;
	virtual bool           CollisionEnabled() const = 0;
	virtual void           EnableCollision(bool bEnable) = 0;
};

class MCollisionManager : public TSingleton<MCollisionManager>
{

public:
	/**
	 * 충돌체를 매니저에 등록 (여기에 등록된 충돌체들만 충돌 검사를 수행)
	 * @param InCollision 충돌체 인터페이스
	 */
	void EnrollCollision(ICollision* InCollision);

	/**
	 * 충돌체를 매니저에서 제거
	 * @param InCollision 충돌체 인터페이스
	 */
	void UnEnrollCollision(ICollision* InCollision);

	void UnEnrollAllCollision();

	/**
	 * 충돌 레이어 설정 (레이어와 레이어간 충돌 여부 설정)
	 * @param InLeft 소스 레이어
	 * @param InRight 타겟 레이어
	 * @param bEnable 충돌 여부
	 */
	void SetCollisionLayer(ETraceType InLeft, ETraceType InRight, bool bEnable);

	JArray<ICollision*>& GetLayer(ETraceType InType);

	/**
	 * 월드 충돌 업데이트 (매 프레임 호출)
	 */
	void UpdateCollision();

private:
	void UpdateLayerCollision(ETraceType InRow, ETraceType InCol);
	void CheckCollision(ICollision* InLeft, ICollision* InRight);
	bool Intersect(ICollision* InLeft, ICollision* InRight, FHitResult& OutHitResult);

private:
	JHash<ETraceType, JArray<ICollision*>> mLayerHash;		// 레이어별 충돌체 리스트
	JHash<uint64_t, bool>                  mCollisionHash;	// 충돌체간 충돌 여부 해시

	std::bitset<EnumAsByte(ETraceType::Max)> mCollisionLayer[EnumAsByte(ETraceType::Max)];	// 충돌 레이어

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MCollisionManager>;
	friend class GUI_Settings_Collision;
	MCollisionManager();
	~MCollisionManager();

public:
	MCollisionManager(const MCollisionManager&)            = delete;
	MCollisionManager& operator=(const MCollisionManager&) = delete;
#pragma endregion
};
