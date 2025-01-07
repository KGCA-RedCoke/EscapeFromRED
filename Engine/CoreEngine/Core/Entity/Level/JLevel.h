#pragma once
#include "Core/Entity/JObject.h"
#include "Core/Entity/UI/MUIManager.h"
#include "Core/Manager/MPoolManager.h"
#include "Core/Utils/SpaceDivision/SpaceDivision.h"
#include "Game/Src/Enemy/AEnemy.h"

class JUIComponent;
class AEnemy;
class JWidgetComponent;
namespace Quad
{
	class JTree;
}
class AActor;


class JKihyunDialog : public JWidgetComponent
{
public:
	JKihyunDialog();
	~JKihyunDialog() override;
};

DECLARE_DYNAMIC_DELEGATE(FOnLevelLoaded);

DECLARE_DYNAMIC_DELEGATE(FOnQuestStart, uint32_t QuestID)

DECLARE_DYNAMIC_DELEGATE(FOnQuestEnd, uint32_t QuestID);

/**
 * 레벨에서는 액터 관리(환경, 배치)를 담당한다.
 */
class JLevel : public JObject
{
public:
	FOnLevelLoaded OnLevelLoaded;
	FOnQuestStart  OnQuestStart;
	FOnQuestEnd    OnQuestEnd;

public:
	JLevel();
	JLevel(const JText& InPath, bool bUseTree = true);
	~JLevel() override;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	int32_t GetActorCount() const { return mActors.size(); }

public:
	virtual void InitializeLevel();
	virtual void UpdateLevel(float DeltaTime);
	virtual void RenderLevel();

public:
	void AddWidgetComponent(JWidgetComponent* InWidgetComponent) { mWidgetComponents.push_back(InWidgetComponent); }
	bool IsLoaded() const { return bThreadLoaded; }

public:
	// void AddActor(const Ptr<AActor>& InActor);

	AActor* LoadActorFromPath(const JText& InPath);

	void ClearLevel();

	template <typename T, typename... Args>
	T* CreateActor(JTextView InName, Args&&... InArgs);

public:
	void ShowPressEKey(bool bShow);
	void SetHPBar(uint32_t InIndex, bool bShow) const;
	void IncreaseHPBar(uint32_t InIndex);
	void DecreamentHPBar(uint32_t InIndex);

public:
	bool bThreadLoaded = false;

	UPtr<Quad::JTree> mOcTree;

	JArray<UPtr<class AActor>> mActors;	// 레벨에 속한 액터들
	JArray<UPtr<class AActor>> mReservedActors;	// 예약된 액터들
	JHash<int32_t, int32_t>    mActorIndexMap;	// 액터 인덱스 맵 (FNode PinID, FNode Actor Index)
	MPoolManager<AEnemy>       mEnemyPool;

	// Widget
	JArray<class JWidgetComponent*> mWidgetComponents;	// 레벨에 속한 UI 컴포넌트들
	class APlayerCharacter*         mPlayerCharacter;
	UPtr<class JKihyunDialog>       mKihyunDialog;
	JUIComponent*                   mPressEKey;
	JUIComponent*                   mHPBar[3];

	JArray<AActor*> EnemySpawner;
	JArray<APawn*>  Pawns;


	friend class GUI_Inspector;
	friend class GUI_Viewport;
};

REGISTER_CLASS_TYPE(JLevel)

template <typename T = AActor, typename... Args>
T* JLevel::CreateActor(JTextView InName, Args&&... InArgs)
{
	static_assert(std::is_base_of<AActor, T>::value, "T must be derived from AActor");

	UPtr<T> newActor = MakeUPtr<T>(InName, std::forward<Args>(InArgs)...);

	T* ptr = newActor.get();

	mReservedActors.push_back(std::move(newActor));

	return ptr;
}
