#pragma once
#include "enums.h"
#include "Core/Utils/Math/Vector2.h"

using FKeyboardData = Jacob::Input::KeyState;

DECLARE_DYNAMIC_DELEGATE(FOnKeyPressed, float)

class XKeyboardMouse
{
public:
	XKeyboardMouse();
	~XKeyboardMouse() = default;

public:
	void Initialize();
	void Update(float DeltaTime);

public:
	FORCEINLINE bool                   IsKeyDown(const EKeyCode Key) const { return IsKeyDown_Internal(Key); }
	FORCEINLINE bool                   IsKeyUp(const EKeyCode Key) const { return IsKeyUp_Internal(Key); }
	FORCEINLINE bool                   IsKeyPressed(const EKeyCode Key) const { return IsKeyPressed_Internal(Key); }
	FORCEINLINE const JMath::TVector2& GetMousePosition() const { return mMousePosition; }
	FORCEINLINE Jacob::Platform::Point GetCurMouseDelta() const { return mCurrentMouseDelta; }

public:
	void AddInputBinding(const EKeyCode Key, const EKeyState KeyState,const FOnKeyPressed::FunctionType& Delegate);

private:
	void CreateKeys();

	void UpdateKeys();
	void UpdateKey(FKeyboardData& InKey);

	void UpdateMouse();

	void UpdateBindings(const float DeltaTime);

	void ClearKeys();
	void ClearMouse();

private:
	inline bool IsKeyDown_Internal(EKeyCode Key) const
	{
		return mKeys[static_cast<UINT>(Key)].State == EKeyState::Down;
	}

	inline bool IsKeyUp_Internal(EKeyCode Key) const
	{
		return mKeys[static_cast<UINT>(Key)].State == EKeyState::Up;
	}

	inline bool IsKeyPressed_Internal(EKeyCode Key) const
	{
		return mKeys[static_cast<UINT>(Key)].State == EKeyState::Pressed;
	}


public:
	JHash<EKeyCode, FOnKeyPressed> OnKeyPressed;
	JHash<EKeyCode, FOnKeyPressed> OnKeyReleased;
	JHash<EKeyCode, FOnKeyPressed> OnKeyHold;

private:
	JArray<FKeyboardData> mKeys;
	JMath::TVector2       mLastMousePosition;
	JMath::TVector2       mMousePosition;


	Jacob::Platform::Point mCurrentMouseDelta{};
	Jacob::Platform::InputSnapshot mInputSnapshot;
	bool mHasMousePosition = false;
};
