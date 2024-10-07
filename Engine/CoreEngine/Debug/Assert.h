#pragma once
#include <comdef.h>
#include <source_location>

#include "common_include.h"
#include "Core/Utils/Logger.h"
#include "imgui/imgui.h"


/**
 * 반환 된 HResult값을 판단 Succeeded에서 실패하면 프로그램 자동 종료 및 로그 남김
 * (아랫단의 함수 호출 실패(주요 기능)이므로 프로그램 종료)
 * @param InResult HResult 반환 값
 * @param InLocation 이 메서드 호출 위치
 */
inline void CheckResult(HRESULT InResult, const std::source_location& InLocation = std::source_location::current())
{
	if (!SUCCEEDED(InResult))
	{
		_com_error err(InResult);
		LPCTSTR    errMsg = err.ErrorMessage();

		LOG_CORE_ERROR("Function:{0}, Line:{1}, Error:{2}",
					   InLocation.file_name(),
					   InLocation.line(),
					   WString2String( errMsg));

		assert(false);
	}
}

/**
 * 런타임 실행 중 발생하는 에러시 imgui 팝업을 띄워준다.
 * 런타임에 실행했을 떄 실패해도 프로그램이 종료되지 않는다.
 * @param InMessage 확인 메시지
 * @param InTitle 팝업 타이틀
 */
inline void ShowErrorPopup(const JText& InMessage, const JText& InTitle = "Error")
{
	ImGui::OpenPopup(InTitle.c_str());

	if (ImGui::BeginPopupModal(InTitle.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("%s", InMessage.c_str());
		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// Win API 
	// MessageBox(nullptr, InMessage.c_str(), InTitle.c_str(), MB_OK | MB_ICONERROR);
}
