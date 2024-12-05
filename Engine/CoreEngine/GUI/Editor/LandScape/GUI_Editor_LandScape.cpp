#include "GUI_Editor_LandScape.h"

#include "Core/Entity/Level/MLevelManager.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Window/Application.h"

GUI_Editor_LandScape::GUI_Editor_LandScape(const JText& InTitle)
	: GUI_Base(InTitle) {}

GUI_Editor_LandScape::~GUI_Editor_LandScape() {}

void GUI_Editor_LandScape::Render()
{
	GUI_Base::Render();
}

void GUI_Editor_LandScape::Update_Implementation(float DeltaTime)
{
	ImGui::DragInt("Columns", &mLandScapeDesc.Column, 1, 1, 100);
	ImGui::DragInt("Rows", &mLandScapeDesc.Row, 1, 1, 100);
	ImGui::DragFloat("Width", &mLandScapeDesc.CellDistance, 1.f, 1.f, 1000.f);
	ImGui::DragFloat("Height", &mLandScapeDesc.ScaleHeight, 1.f, 1.f, 1000.f);

	if (ImGui::BeginCombo("##AlbedoMap", WString2String(mAlbedoTexture).c_str()))
	{
		const JArray<JTexture*> loaded = GetWorld.TextureManager->GetManagedList();
		for (auto& loadedTexture : loaded)
		{
			JText texPath = WString2String(loadedTexture->GetTextureName());
			if (ImGui::Selectable(texPath.c_str()))
			{
				mAlbedoTexture = loadedTexture->GetTextureName();
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("##HeightMap", WString2String(mHeightTexture).c_str()))
	{
		const JArray<JTexture*> loaded = GetWorld.TextureManager->GetManagedList();
		for (auto& loadedTexture : loaded)
		{
			JText texPath = WString2String(loadedTexture->GetTextureName());
			if (ImGui::Selectable(texPath.c_str()))
			{
				mHeightTexture = loadedTexture->GetTextureName();
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("##NormalMap", WString2String(mNormalTexture).c_str()))
	{
		const JArray<JTexture*> loaded = GetWorld.TextureManager->GetManagedList();
		for (auto& loadedTexture : loaded)
		{
			// JText texPath = WString2String(loadedTexture->GetPath());
			// if (ImGui::Selectable(texPath.c_str()))
			// {
			// 	mNormalTexture = loadedTexture->GetPath();
			// }
		}
		ImGui::EndCombo();
	}

	ImGui::Separator();

	if (ImGui::Button("Generate LandScape"))
	{
		GenerateLandScape();
	}
}

void GUI_Editor_LandScape::GenerateLandScape()
{
	auto newLandScape             = MakeUPtr<JLandScape>("LandScape");
	newLandScape->mMapDescription = mLandScapeDesc;

	newLandScape->Initialize();

	if (!mAlbedoTexture.empty())
	{
		newLandScape->mAlbedoMap = GetWorld.TextureManager->Load(mAlbedoTexture);
	}
	if (!mHeightTexture.empty())
	{
		newLandScape->mHeightMap = GetWorld.TextureManager->Load(mHeightTexture);
		newLandScape->mHeightMap->SetEditable();
	}
	if (!mNormalTexture.empty())
	{
		newLandScape->mNormalMap = GetWorld.TextureManager->Load(mNormalTexture);
	}

	newLandScape->GenerateLandScape();

	GetWorld.LevelManager->GetActiveLevel()->mActors.push_back(std::move(newLandScape));
}
