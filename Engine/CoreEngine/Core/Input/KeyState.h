#pragma once

#include "Platform/Window.h"
#include <chrono>

namespace Jacob::Input
{
enum class KeyPhase : std::uint8_t { None, Up, Down, Pressed };

// Input transitions are engine logic, independent of Win32/SDL polling.
struct KeyState
{
    Platform::Key Key;
    KeyPhase State = KeyPhase::None;
    std::chrono::steady_clock::time_point PressTime{};
    std::chrono::steady_clock::time_point ReleaseTime{};
    std::chrono::milliseconds PressDuration{0};

    void Update(bool down, std::chrono::steady_clock::time_point now)
    {
        if (down)
        {
            if (State == KeyPhase::None || State == KeyPhase::Up)
            {
                State = KeyPhase::Down;
                PressTime = now;
                PressDuration = std::chrono::milliseconds(0);
            }
            else
            {
                State = KeyPhase::Pressed;
                PressDuration = std::chrono::duration_cast<std::chrono::milliseconds>(now - PressTime);
            }
        }
        else
        {
            const bool wasDown = State == KeyPhase::Down || State == KeyPhase::Pressed;
            State = wasDown ? KeyPhase::Up : KeyPhase::None;
            if (wasDown) ReleaseTime = now;
            PressDuration = std::chrono::milliseconds(0);
        }
    }
};
}
