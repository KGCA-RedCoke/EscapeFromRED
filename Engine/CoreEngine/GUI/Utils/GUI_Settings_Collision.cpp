#include "GUI_Settings_Collision.h"

#include "Core/Entity/Component/Scene/Shape/Collision/MCollisionManager.h"
#include "Core/Interface/JWorld.h"

GUI_Settings_Collision::GUI_Settings_Collision(const JText& InTitle)
	: GUI_Base(InTitle) {}

void GUI_Settings_Collision::Initialize()
{
	GUI_Base::Initialize();
}

void GUI_Settings_Collision::Update_Implementation(float DeltaTime)
{
	if (ImGui::Checkbox("Show Collision Layer", &GetWorld.bDebugMode))
	{
		LOG_CORE_INFO("Show Collision Layer: {}", GetWorld.bDebugMode);
	}

	// 테이블 생성
	if (ImGui::BeginTable("CollisionLayerTable",
						  EnumAsByte(ETraceType::Max) + 1,
						  ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
	{
		// 첫 번째 행: 헤더 (가로 레이어 이름)
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Layer");
		for (int col = 0; col < EnumAsByte(ETraceType::Max); ++col)
		{
			ImGui::TableSetColumnIndex(col + 1);
			ImGui::Text(TraceTypeToString(static_cast<ETraceType>(col)));
		}

		// 나머지 행: 체크박스
		for (int row = 0; row < EnumAsByte(ETraceType::Max); ++row)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(TraceTypeToString(static_cast<ETraceType>(row))); // 세로 레이어 이름

			for (int col = 0; col < EnumAsByte(ETraceType::Max); ++col)
			{
				ImGui::TableSetColumnIndex(col + 1);

				// test 함수는 C++의 std::bitset 클래스의 멤버 함수입니다. 이 함수는 지정된 비트 위치의 값을 반환
				bool isChecked = MCollisionManager::Get().mCollisionLayer[row].test(col);
				if (ImGui::Checkbox(("##Layer" + std::to_string(row) + "_" + std::to_string(col)).c_str(),
									&isChecked))
				{
					// 체크박스 상태 업데이트
					MCollisionManager::Get().mCollisionLayer[row].set(col, isChecked);
				}
			}
		}

		ImGui::EndTable();
	}
}
