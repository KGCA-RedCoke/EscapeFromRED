#pragma once

#include "Platform/Window.h"
#include <windows.h>
#include <stdexcept>

namespace Jacob::Platform
{
inline HWND Win32Handle(NativeWindowHandle handle)
{
    if (handle.kind != NativeWindowKind::Win32 || !handle.value)
        throw std::logic_error("A Win32 window is required by this backend.");
    return static_cast<HWND>(handle.value);
}
}
