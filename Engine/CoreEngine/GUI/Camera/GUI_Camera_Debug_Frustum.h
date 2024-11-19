#pragma once
#include <wrl/client.h>

#include "GUI/GUI_Base.h"

class GUI_Camera_Debug_Frustum : public GUI_Base
{
public:
	GUI_Camera_Debug_Frustum(const JText& InTitle);

public:
	void Update_Implementation(float DeltaTime) override;
	void Render() override;

private:
	void Initialize() override;

private:
	class JCamera*                                         mCamera;	// 뷰포트 카메라
	class FViewportData*                                   mViewport;	// 뷰포트
	float                                                  mDeltaTime     = 0.f;
	Microsoft::WRL::ComPtr<class ID3D11ShaderResourceView> mSampleTexture = nullptr;
};
