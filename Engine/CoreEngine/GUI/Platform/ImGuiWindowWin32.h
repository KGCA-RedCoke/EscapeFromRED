#pragma once
class Window;
// Legacy editor bridge. The platform library itself does not depend on ImGui.
void InitializeImGuiWindowWin32(Window& window);
