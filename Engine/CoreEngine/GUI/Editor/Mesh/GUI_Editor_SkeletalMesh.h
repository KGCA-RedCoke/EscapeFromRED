#pragma once
#include "GUI/Editor/GUI_Editor_Base.h"

class GUI_Editor_SkeletalMesh : public GUI_Editor_Base
{
public:
public:
	/** GUI Title로 메시 경로를 전달 */
	GUI_Editor_SkeletalMesh(const JText& InTitle);
	~GUI_Editor_SkeletalMesh() override = default;

	void Render() override;

private:
	void Initialize() override;
	void Update(float DeltaTime) override;
	void Update_Implementation(float DeltaTime) override;

private:
	void DrawViewport() const;
	void DrawProperty() const;
	void DrawMaterialSlot() const;
	void DrawAnimationPreview() const;
	void DrawSaveButton() const;

private:
	WPtr<class JSkeletalMeshObject> mMeshObject;
};
