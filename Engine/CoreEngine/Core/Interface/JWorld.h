#pragma once
#include "Core/Entity/Actor/AActor.h"
#include "Core/Entity/Level/MLevelManager.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Manager/Manager_Base.h"
#include "Core/Thread/ThreadPool.h"
#include "Core/Utils/Math/Vector.h"

DECLARE_DYNAMIC_DELEGATE(FOnDebugModeChanged, bool);

#define GetWorld JWorld::Get()

class AActor;
class JLevel;

class JWorld : public TSingleton<JWorld>
{
public:
	FOnDebugModeChanged OnDebugModeChanged;

public:
	void Initialize();
	void Update(float DeltaTime);
	void Render();
	void Release();

public:
	[[nodiscard]] float GetDeltaSeconds() const;
	[[nodiscard]] float GetGameTime() const;

	template <class T, typename... Args>
	T* SpawnActor(const JText&   InName,
				  const FVector& InLocation = FVector::ZeroVector,
				  const FVector& InRotation = FVector::ZeroVector,
				  AActor*        InOwner    = nullptr,
				  Args&&...      InArgs)
	{
		T* newActor = LevelManager->GetActiveLevel()->CreateActor<T>(InName, std::forward<Args>(InArgs)...);
		newActor->SetWorldLocation(InLocation);
		newActor->SetWorldRotation(InRotation);
		newActor->SetOwnerActor(InOwner);

		return newActor;
	}

private:
	static void SearchFiles_Recursive(const std::filesystem::path& InPath);
	static void ParseFiles_Recursive(const std::filesystem::path& InPath);

public:
	class Application*              Application;			// 애플리케이션
	class MGUIManager*              GUIManager;				// GUI
	class MViewportManager*         ViewportManager;		// 뷰포트
	class MCameraManager*           CameraManager;			// 카메라
	class MShaderManager*           ShaderManager;			// 셰이더
	class MTextureManager*          TextureManager;			// 텍스처
	class MMaterialManager*         MaterialManager;		// 머티리얼
	class MMaterialInstanceManager* MaterialInstanceManager;// 머티리얼 인스턴스
	class MMeshManager*             MeshManager;			// 메시
	class MLevelManager*            LevelManager;			// 레벨
	class MSoundManager*            SoundManager;			// 사운드
	class MUIManager*               UIManager;				// UI
	class MAnimataionManager*       AnimationManager;		// 애니메이션
	class XD3DDevice*               D3D11API;				// 디바이스


public:
	struct
	{
		Buffer::FBuffer_Light_Point Data[127];
		uint32_t                    Count = 0;
	} WorldPointLightData;

	struct
	{
		Buffer::FBuffer_Light_Spot Data[127];
		uint32_t                   Count = 0;
	} WorldSpotLightData;

	Thread::ThreadPool ThreadPool;		// 스레드

#pragma region Singleton Boilerplate

public:
	friend class TSingleton<JWorld>;

	JWorld();

public:
	JWorld(const JWorld&)            = delete;
	JWorld& operator=(const JWorld&) = delete;

#pragma endregion
};
