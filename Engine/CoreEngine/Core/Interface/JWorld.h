#pragma once
#include "Core/Manager/Manager_Base.h"
#include "Core/Thread/ThreadPool.h"
#include "Core/Utils/Math/Vector.h"

#define GetWorld JWorld::Get()

class AActor;
class JLevel;

class JWorld : public TSingleton<JWorld>
{
public:
	void Initialize();
	void Update(float DeltaTime);
	void Render();
	void Release();

public:
	[[nodiscard]] float GetDeltaSeconds() const;
	[[nodiscard]] float GetGameTime() const;

	AActor* SpawnActor(const JText&   InName,
					   const FVector& InLocation = FVector::ZeroVector,
					   const FVector& InRotation = FVector::ZeroVector,
					   AActor*        InOwner    = nullptr,
					   JLevel*        InLevel    = nullptr);

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
	class MActorManager*            ActorManager;			// 액터
	class MLevelManager*            LevelManager;			// 레벨
	class XD3DDevice*               D3D11API;				// 디바이스


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
