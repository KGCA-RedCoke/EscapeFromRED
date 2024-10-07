#pragma once
#include "common_include.h"
#include "Core/Entity/Level/JLevel.h"
#include "Core/Utils/Math/Vector4.h"

#define GetWorld() JWorld::Get()

class JWorld : public TSingleton<JWorld>
{
public:
	void Initialize();
	void Update(float DeltaTime);
	void Release();

	void Draw();

	template <typename T, typename... Args>
	T* SpawnActor(Args&&... InArgs)
	{
		JActor* newActorPtr = mActiveLevel->CreateActor(std::forward<Args>(InArgs)...);

		return newActorPtr;
	}

private:
	JArray<UPtr<class JLevel>> mLevels;
	JLevel*                    mActiveLevel;


#pragma region Singleton Boilerplate

private:
	friend class TSingleton<JWorld>;

	JWorld();
	~JWorld();

public:
	JWorld(const JWorld&)            = delete;
	JWorld& operator=(const JWorld&) = delete;

#pragma endregion
};
