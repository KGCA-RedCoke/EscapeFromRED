#include "XKeyboardMouse.h"

#include "Core/Window/Window.h"

XKeyboardMouse::XKeyboardMouse()
{}

void XKeyboardMouse::Initialize()
{
	CreateKeys();

	ClearKeys();
	ClearMouse();
}

void XKeyboardMouse::Update(float DeltaTime)
{
	mInputSnapshot = Window::GetWindow()->ReadInput();
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
    if (!mInputSnapshot.focused)
    {
        ClearKeys(); // Once per frame, so release events survive until UpdateBindings.
        return;
    }
	for (FKeyboardData& key : mKeys)
	{
		UpdateKey(key);
	}
}

void XKeyboardMouse::UpdateKey(FKeyboardData& InKey)
{
    InKey.Update(mInputSnapshot.IsDown(InKey.Key), std::chrono::steady_clock::now());
}

void XKeyboardMouse::UpdateMouse()
{
    const auto position = mInputSnapshot.clientPosition;
    mCurrentMouseDelta = {};
    if (mHasMousePosition && mInputSnapshot.focused)
    {
        mCurrentMouseDelta.x = position.x - static_cast<int>(mLastMousePosition.x);
        mCurrentMouseDelta.y = position.y - static_cast<int>(mLastMousePosition.y);
    }
    mLastMousePosition.x = static_cast<float>(position.x);
    mLastMousePosition.y = static_cast<float>(position.y);
    mMousePosition = mLastMousePosition;
    mHasMousePosition = mInputSnapshot.focused;
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
    const auto now = std::chrono::steady_clock::now();
    for (FKeyboardData& key : mKeys) key.Update(false, now);
}

void XKeyboardMouse::ClearMouse()
{
    mHasMousePosition = false;
    mCurrentMouseDelta = {};
}
