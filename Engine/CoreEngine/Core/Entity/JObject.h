﻿#pragma once
#include "common_include.h"
#include "Core/Manager/IManagedInterface.h"
#include "Core/Manager/MClassFactory.h"
#include "Core/Utils/FileIO/JSerialization.h"

enum EObjectFlags : uint32_t
{
	None          = 0,
	MarkAsDirty   = 1 << 0, // 에셋 변경사항이 있음
	IsVisible     = 1 << 1, // 렌더링 가능한 상태
	IsValid       = 1 << 2, // 업데이트 가능한 상태
	IsPendingKill = 1 << 3, // 소멸 대기 상태
	DontDestroy   = 1 << 4, // 소멸하지 않는 상태 (레벨 전환 시 소멸되지 않음)
	IgnoreFrustum = 1 << 5, // 카메라 프러스텀 체크 무시
	ShouldTick    = 1 << 6  // Tick 함수 호출 여부
};

constexpr const char* NAME_OBJECT_FLAGS[] =
{
	"MarkAsDirty",
	"IsVisible",
	"IsValid",
	"PendingKill",
	"DontDestroy",
	"IgnoreFrustum",
	"ShouldTick"
};

class JObject : public JAsset,
				public IManagedInterface
{
public:
	JObject();
	explicit JObject(JTextView InName);
	JObject(const JObject& Copy);
	~JObject() override;

public:
	uint32_t                GetHash() const override;
	UPtr<IManagedInterface> Clone() const override;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	virtual void Initialize() {}
	virtual void BeginPlay() {}
	virtual void Tick(float DeltaTime) {};

	virtual void Destroy()
	{
		mObjectFlags |= EObjectFlags::IsPendingKill;
	}

	JText GetObjectType() const { return mObjectType; }

	[[nodiscard]] bool IsVisible() const { return mObjectFlags & EObjectFlags::IsVisible; }
	[[nodiscard]] bool IsValid() const { return mObjectFlags & EObjectFlags::IsValid; }
	[[nodiscard]] bool IsPendingKill() const { return mObjectFlags & EObjectFlags::IsPendingKill; }
	[[nodiscard]] bool IsMarkedAsDirty() const { return mObjectFlags & EObjectFlags::MarkAsDirty; }
	[[nodiscard]] bool IsDontDestroy() const { return mObjectFlags & EObjectFlags::DontDestroy; }
	[[nodiscard]] bool IsIgnoreFrustum() const { return mObjectFlags & EObjectFlags::IgnoreFrustum; }
	[[nodiscard]] bool ShouldTick() const { return mObjectFlags & EObjectFlags::ShouldTick; }

	void MarkAsDirty() { mObjectFlags |= EObjectFlags::MarkAsDirty; }

	void SetVisible(bool bVisible)
	{
		bVisible ? mObjectFlags |= EObjectFlags::IsVisible : mObjectFlags &= ~EObjectFlags::IsVisible;
	}

	void SetFlag(uint32_t InFlag)
	{
		mObjectFlags |= InFlag;
	}

	void RemoveFlag(uint32_t InFlag)
	{
		mObjectFlags &= ~InFlag;
	}

	[[nodiscard]] FORCEINLINE JText GetName() const { return mName; }

protected:
	JText    mName;
	JText    mObjectType  = NAME_OBJECT_BASE;
	uint32_t mObjectFlags = 0;

protected:
	static uint32_t g_DefaultObjectNum;
};

REGISTER_CLASS_TYPE(JObject);
