#pragma once
#include "Core/Graphics/Material/JMaterial.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "GUI/GUI_Base.h"

// Forward Declaration
class JCamera;
class JMeshObject;
class JMaterial;
class JShader;
class GUI_Viewport;

/**
 * 머티리얼 에디터 
 */
class GUI_Editor_Material : public GUI_Base
{
public:
	GUI_Editor_Material(const JText& InTitle);
	~GUI_Editor_Material() override;

	void Render() override;

private:
	void Update_Implementation(float DeltaTime) override;

public:
	static int32_t GetAvailableIndex() { return s_CachedMaterialEditorIndex; }

private:
	void ShowMenuBar() override;

	void ShowMaterialEditor();
	void ShowFileBrowser();

	void ShowParamPopup();

	void ShowTextureSlot(const Ptr<JMaterial>& InMaterialData, EMaterialFlag Flags, const char* TextureType) const;

private:
	inline static int32_t s_MaterialEditorNum         = 0;	// 생성된 Material Editor 개수
	inline static int32_t s_CachedMaterialEditorIndex = 0; // 캐시된 Material Editor  

	bool bOpenFileBrowser;
	bool bShowNewParam;
	bool bOpenFolder;

	char mFilePath[256];

	Ptr<JCamera>       mCamera;			// 뷰포트 카메라
	Ptr<FViewportData> mViewport;		// 뷰포트
	Ptr<JMeshObject>   mSphere;			// 머티리얼 적용 표면
	Ptr<JMaterial>     mMaterialToEdit;	// 머티리얼
	Ptr<JShader>       mShader;			// 셰이더

	float mDeltaTime;
};
