#pragma once
#include "JShader.h"
#include "Core/Manager/Manager_Base.h"

class JShader_Shadow;
class JShader_UI;
class JShader_Basic;

class MShaderManager : public Manager_Base<JShader, MShaderManager>
{
public:
	void UpdateCamera(class JCameraComponent* InCameraObj);

	void UpdateShader(ID3D11DeviceContext* InDeviceContext, JShader* InShader);

private:
	void Initialize_Initialize();

public:
	JShader_Basic*    BasicShader;	// 기본(Default) 셰이더
	JShader*          IDShader;		// 마우스 피킹 셰이더
	JShader*          UIElementShader;	// UI 요소 셰이더
	JShader*          ColorShader;	// 색상 셰이더
	JShader_UI*       UIShader;	// UI 셰이더
	JShader_Shadow*   ShadowMapShader;	// 그림자 맵 셰이더
	JShader*          mCachedShader;	// 현재 설정된 셰이더
	JCameraComponent* mCachedCamera;	// 현재 설정된 카메라

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MShaderManager>;
	friend class JWorld;

	MShaderManager();
	~MShaderManager() = default;

public:
	MShaderManager(const MShaderManager&)            = delete;
	MShaderManager& operator=(const MShaderManager&) = delete;
#pragma endregion
};
