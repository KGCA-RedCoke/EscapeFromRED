#include "GUI_Base.h"

#include "Core/Interface/JWorld.h"
#include "imgui/imgui_internal.h"

GUI_Base::GUI_Base()
{}

GUI_Base::GUI_Base(const JText& InTitle)
	: mTitle(InTitle),
	  bIsWindowOpen(true)
{}

void GUI_Base::Update(float DeltaTime)
{
	if (!bIsWindowOpen)
		return;

	if (GetWorld.bGameMode)
	{
		// // // 화면 전체 크기와 위치를 설정
		// ImGui::SetNextWindowPos(ImVec2(0, 0));
		// ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		//
		// // 창 스타일을 풀스크린 전용으로 변경 (타이틀 바, 메뉴바 제거)
		mWindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoResize;
		ImGui::GetStyle().WindowRounding = 0.0f;// <- Set this on init or use ImGui::PushStyleVar()
		ImGui::GetStyle().ChildRounding = 0.0f;
		ImGui::GetStyle().FrameRounding = 0.0f;
		ImGui::GetStyle().GrabRounding = 0.0f;
		ImGui::GetStyle().PopupRounding = 0.0f;
		ImGui::GetStyle().ScrollbarRounding = 0.0f;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
	}


	ImGui::Begin(mTitle.c_str(), &bIsWindowOpen, mWindowFlags);
	{
		bIsVisible = ImGui::IsItemVisible();

		if (!ImGui::IsWindowCollapsed())
		{
			ChangeWindowStyleIfNotDocked();

			ShowMenuBar();

			Update_Implementation(DeltaTime);
		}


		ImGui::End();
	}
}

bool GUI_Base::IsWindowDocked() const
{
	const ImGuiID        dockNodeID = ImGui::GetID(mTitle.c_str());
	const ImGuiDockNode* node       = ImGui::DockBuilderGetNode(dockNodeID);

	return node != nullptr;
}

void GUI_Base::OpenIfNotOpened()
{
	if (!bIsWindowOpen)
	{
		bIsWindowOpen = true;
	}
}

void GUI_Base::ChangeWindowStyleIfNotDocked()
{
	if (!IsWindowDocked())
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 45.5f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}
}
