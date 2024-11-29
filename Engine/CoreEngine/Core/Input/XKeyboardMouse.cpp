#include "XKeyboardMouse.h"

#include "Core/Window/Window.h"

void XKeyboardMouse::Initialize()
{
	CreateKeys();

	ClearKeys();
	ClearMouse();
}

void XKeyboardMouse::Update(float DeltaTime)
{
	UpdateKeys();
	UpdateMouse();

	UpdateBindings(DeltaTime);
}

void XKeyboardMouse::AddInputBinding(const EKeyCode                     Key, const EKeyState KeyState,
									 const FOnKeyPressed::FunctionType& Delegate)
{
	switch (KeyState)
	{
	case EKeyState::None:
		LOG_CORE_WARN("Key State is None. Please Check Key State.");
		return;
	case EKeyState::Up:
		OnKeyReleased[Key].Bind(Delegate);
		break;
	case EKeyState::Down:
		OnKeyPressed[Key].Bind(Delegate);
		break;
	case EKeyState::Pressed:
		OnKeyHold[Key].Bind(Delegate);
		break;
	}
}

void XKeyboardMouse::CreateKeys()
{
	mKeys.clear();
	mKeys.reserve(static_cast<uint8_t>(EKeyCode::End));
	OnKeyPressed.reserve(30);
	OnKeyReleased.reserve(30);
	OnKeyHold.reserve(30);

	for (size_t i = 0; i < static_cast<uint8_t>(EKeyCode::End); ++i)
	{
		mKeys.emplace_back(static_cast<EKeyCode>(i), EKeyState::None);
	}
}

void XKeyboardMouse::UpdateKeys()
{
	for (FKeyboardData& key : mKeys)
	{
		UpdateKey(key);
	}
}

void XKeyboardMouse::UpdateKey(FKeyboardData& InKey)
{
	if (GetFocus()) // 현재 윈도우에 포커싱 중?
	{
		IsKeyDown_Impl(InKey.Key) ? UpdateKeyDown(InKey) : UpdateKeyUp(InKey);
	}
	else // 포커싱이 다른 윈도우에 있으면 모든 Input Clear
	{
		ClearKeys();
	}
}

void XKeyboardMouse::UpdateKeyDown(FKeyboardData& InKey)
{
	if (InKey.State <= EKeyState::Up)
	{
		InKey.State     = EKeyState::Down;
		InKey.PressTime = std::chrono::steady_clock::now();
	}
	else
	{
		InKey.State         = EKeyState::Pressed;
		InKey.PressDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
			 std::chrono::steady_clock::now() - InKey.PressTime);
	}
}

void XKeyboardMouse::UpdateKeyUp(FKeyboardData& InKey)
{
	if (InKey.State == EKeyState::Pressed)
	{
		InKey.State         = EKeyState::Up;
		InKey.ReleaseTime   = std::chrono::steady_clock::now(); // TODO Release 필요하면 구현
		InKey.PressDuration = std::chrono::milliseconds(0);
	}
	else
	{
		InKey.State = EKeyState::None;
	}
}

void XKeyboardMouse::UpdateMouse()
{
	// 현재 커서 좌표 받아오기
	POINT mousePoint;
	GetCursorPos(&mousePoint);

	mCurrentMouseDelta.x = mousePoint.x - mLastMousePosition.x;
	mCurrentMouseDelta.y = mousePoint.y - mLastMousePosition.y;

	mLastMousePosition = mousePoint;

	ScreenToClient(Window::GetWindow()->GetWindowHandle(), &mousePoint); // 커서의 좌표를 클라이언트 윈도우 기준으로 잡아준다.

	mMousePosition.x = mousePoint.x;
	mMousePosition.y = mousePoint.y;
}

void XKeyboardMouse::UpdateBindings(const float DeltaTime)
{
	for (auto& [key, delegate] : OnKeyPressed)
	{
		if (IsKeyDown(key))
		{
			delegate.Execute(DeltaTime);
		}
	}

	for (auto& [key, delegate] : OnKeyReleased)
	{
		if (IsKeyUp(key))
		{
			delegate.Execute(DeltaTime);
		}
	}

	for (auto& [key, delegate] : OnKeyHold)
	{
		if (IsKeyPressed(key))
		{
			delegate.Execute(DeltaTime);
		}
	}
}

void XKeyboardMouse::ClearKeys()
{
	for (FKeyboardData& key : mKeys)
	{
		switch (key.State)
		{
		case EKeyState::Down:
		case EKeyState::Pressed:
			key.State = EKeyState::Up;
			break;
		case EKeyState::Up:
			key.State = EKeyState::None;
			break;
		case EKeyState::None:
			break;
		}

		key.PressDuration = std::chrono::milliseconds(0);
	}
}

void XKeyboardMouse::ClearMouse()
{}
