#include "GUI_Utils.h"

void Utils::GUI::FSearchBar::DrawSearchBar()
{
	// 검색 바 크기 조정
	ImGui::SetNextItemWidth(-FLT_MIN);

	// 바로가기 단축키 설정 (Ctrl + F), 마우스 호버 시 툴팁 표시
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F, ImGuiInputFlags_Tooltip);

	// 기본적 포커스를 받지 않음
	ImGui::PushItemFlag(ImGuiItemFlags_NoNavDefaultFocus, true);

	if (ImGui::InputTextWithHint("##Filter",
								 "incl, -excl (...)",
								 Filter.InputBuf,
								 IM_ARRAYSIZE(Filter.InputBuf),
								 ImGuiInputTextFlags_EscapeClearsAll))
	{
		Filter.Build();
	}
	ImGui::PopItemFlag();
}
