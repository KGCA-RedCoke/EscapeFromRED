#pragma once
#include "Core/Entity/JObject.h"
#include "Core/Utils/SpaceDivision/SpaceDivision.h"

class JWidgetComponent;
namespace Quad
{
	class JTree;
}
class AActor;

DECLARE_DYNAMIC_DELEGATE(FOnLevelLoaded);

/**
 * 레벨에서는 액터 관리(환경, 배치)를 담당한다.
 */
class JLevel : public JObject
{
public:
	FOnLevelLoaded OnLevelLoaded;

public:
	JLevel() = default;
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

	template <typename T, typename... Args>
	T* CreateActor(JTextView InName, Args&&... InArgs);

public:
	bool bThreadLoaded = false;

	UPtr<Quad::JTree> mOcTree;

	JArray<UPtr<class AActor>>      mActors;	// 레벨에 속한 액터들
	JHash<int32_t, int32_t>         mActorIndexMap;	// 액터 인덱스 맵 (FNode PinID, FNode Actor Index)
	JArray<class JWidgetComponent*> mWidgetComponents;	// 레벨에 속한 UI 컴포넌트들

	// TODO : 레벨과 관련된 변수들 추가
	// Time, Mission, States ... etc


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

	mOcTree->Insert(ptr);
	mActors.push_back(std::move(newActor));

	return ptr;
}
