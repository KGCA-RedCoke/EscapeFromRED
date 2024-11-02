#pragma once
#include "GUI_Editor_Base.h"
#include "Core/Graphics/Material/JMaterial.h"
#include "Core/Graphics/Viewport/MViewportManager.h"

// Forward Declaration
class JCamera;
class JMeshObject;
class JMaterial;
class JShader;
class GUI_Viewport;

/**
 * 머티리얼 에디터 
 */
class GUI_Editor_Material : public GUI_Editor_Base
{
public:
	GUI_Editor_Material(const JText& InTitle);
	~GUI_Editor_Material() override = default;

	void Render() override;

private:
	void Update_Implementation(float DeltaTime) override;
	void ShowMenuBar() override;

private:
	void SetMeshObject(JTextView InMeshPath);

	void HandleIntegerType(FMaterialParam& MaterialParam);
	void HandleFloat2Type(FMaterialParam& Param);
	void HandleFloat4Type(FMaterialParam& Param, uint32_t Index) const;

	void ShowMaterialEditor();
	void ShowFileBrowser();

	void ShowTextureSlot(FMaterialParam& Param, uint32_t Index) const;

	void Button_AddParam();

private:
	bool bOpenFileBrowser;
	bool bOpenFolder;
	bool bOpenNewParamPopup = false;

	char mFilePath[256];

	Ptr<JMeshObject>       mPreviewMeshObject;	// 머티리얼 적용 표면
	Ptr<JMaterialInstance> mMaterialToEdit;		// 머티리얼
};
