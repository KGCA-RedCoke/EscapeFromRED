#pragma once
#include "imgui/imgui.h"

namespace Utils::GUI
{
	struct FSearchBar
	{
		ImGuiTextFilter Filter;

		void DrawSearchBar();
	};
}
