﻿#pragma once
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Utils/Math/Vector2.h"
#include "GUI/GUI_Base.h"

class JCamera_Debug;
struct FViewportData;

class GUI_Viewport : public GUI_Base
{
public:
	explicit GUI_Viewport(const JText& InTitle);
	~GUI_Viewport() override = default;

protected:
	void Initialize() override;

protected:
	void Update_Implementation(float DeltaTime) override;

public:
	FORCEINLINE void SetCamera(JCamera_Debug* InCamera) { mEditorCameraRef = InCamera; }

	[[nodiscard]] FORCEINLINE bool IsFocused() const { return bIsFocused; }
	[[nodiscard]] FORCEINLINE bool IsHovered() const { return bIsHovered; }

private:
	void     CreateMousePickingBuffer(ID3D11Device* InDevice);
	void     UpdateMousePickingBuffer(ID3D11DeviceContext* InDeviceContext);
	uint32_t GetMousePickingValue(ID3D11DeviceContext* InDeviceContext, const ImVec2& InMousePos);

protected:
	bool bIsFocused;
	bool bIsHovered;

	JCamera_Debug* mEditorCameraRef;
	FViewportData* mViewportData;

	JText mViewportTitle;
	float mCachedViewportWidth;
	float mCachedViewportHeight;

	struct
	{
		ComPtr<ID3D11Texture2D>         ColorIDBuffer;	// 모든 컬러 아이디를 담는 버퍼
		ComPtr<ID3D11Texture2D>         StagingBuffer;	// CPU에서 특정 위치의 픽셀을 읽기 위한 버퍼
		ComPtr<ID3D11DepthStencilView>  DepthBuffer;		// 깊이 버퍼
		ComPtr<ID3D11DepthStencilState> DepthState;		// 깊이 스테이트
		ComPtr<ID3D11RenderTargetView>  RTV;				// 렌더 타겟 뷰
	} mMousePickingBuffer;
};
