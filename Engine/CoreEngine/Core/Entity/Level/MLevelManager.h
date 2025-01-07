#pragma once
#include "JLevel.h"
#include "Core/Manager/Manager_Base.h"

class MLevelManager : public Manager_Base<JLevel, MLevelManager>
{
	void PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
						void*        Entity) override;

public:
	void Update(float DeltaTime);
	void Render();
	void Destroy();

public:
	JLevel* LoadIntroLevel();
	JLevel* LoadMainLevel();
	JLevel* LoadGameOverLevel();

public:
	JLevel* CreateLevel(const JText& InSavePath);

public:
	FORCEINLINE JLevel* GetActiveLevel() const { return mActiveLevel; }

	void SetActiveLevel(JLevel* InLevel);

private:
	JArray<UPtr<class JLevel>> mLevels;
	JLevel*                    mActiveLevel;

public:
#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MLevelManager>;

	MLevelManager();
	~MLevelManager();

public:
	MLevelManager(const MLevelManager&)            = delete;
	MLevelManager& operator=(const MLevelManager&) = delete;

#pragma endregion
};
