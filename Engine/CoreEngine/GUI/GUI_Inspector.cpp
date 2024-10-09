#include "GUI_Inspector.h"

#include "Core/Entity/Component/Scene/JSceneComponent.h"

GUI_Inspector::GUI_Inspector(const std::string& InTitle)
	: GUI_Base(InTitle) {}

void GUI_Inspector::AddSceneComponent(const JText& InName, const Ptr<JSceneComponent>& InSceneComponent)
{
	mSceneComponents.try_emplace(InName, InSceneComponent);
}

void GUI_Inspector::Update_Implementation(float DeltaTime)
{
	for (auto& element : mSceneComponents)
	{
		if (auto sceneComp = element.second.lock())
		{
			bool bIsDragging = false;

			if (ImGui::CollapsingHeader(sceneComp->GetName().c_str()))
			{
				FVector location = sceneComp->GetLocalLocation();
				FVector rotation = sceneComp->GetLocalRotation();
				FVector scale    = sceneComp->GetLocalScale();

				if (ImGui::DragFloat3("Position", &location.x, 0.01f, -100.f, 100.0f))
				{
					bIsDragging = true;
				}

				if (ImGui::DragFloat3("Rotation", &rotation.x, 0.01f, -360.0f, 360.0f))
				{
					bIsDragging = true;
				}
				if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.f, 10.0f))
				{
					bIsDragging = true;
				}

				if (bIsDragging)
				{
					sceneComp->SetLocalLocation(location);
					sceneComp->SetLocalRotation(rotation);
					sceneComp->SetLocalScale(scale);

					return;
				}
			}

			ImGui::Separator();
		}
	}
}
