#pragma once
#include <wrl/client.h>

#include "GUI/GUI_Base.h"

class GUI_Camera_Debug_Frustum : public GUI_Base
{
public:
	GUI_Camera_Debug_Frustum(const JText& InTitle);
	
public:
	void Update_Implementation(float DeltaTime) override;

private:
	void Initialize() override;

private:
	Microsoft::WRL::ComPtr<class ID3D11ShaderResourceView> mSampleTexture = nullptr;
};
