#pragma once
#include "Core/Entity/Level/JLevel.h"
#include "Core/Entity/UI/MUIManager.h"

DECLARE_DYNAMIC_DELEGATE(FOnQuestStart, uint32_t QuestID);

DECLARE_DYNAMIC_DELEGATE(FOnQuestEnd, uint32_t QuestID);

class JKihyunDialog : public JWidgetComponent
{
public:
	JKihyunDialog();
	~JKihyunDialog() override = default;
};

class JLevel_Main : public JLevel
{
public:
	FOnQuestStart OnQuestStart;
	FOnQuestEnd   OnQuestEnd;

public:
	JLevel_Main();
	~JLevel_Main() override;

public:
	void InitializeLevel() override;
	void UpdateLevel(float DeltaTime) override;
	void RenderLevel() override;

private:
	class APlayerCharacter* mPlayerCharacter;
	UPtr<JKihyunDialog>     mKihyunDialog;
};
