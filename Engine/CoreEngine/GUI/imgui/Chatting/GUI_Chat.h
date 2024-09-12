#pragma once
#include "Core/Manager/Manager_Base.h"

#define G_Chat GUI_Chat::Get()

class GUI_Chat : public TSingleton<GUI_Chat>
{
public:
    void Initialize();
    void Update(float DeltaTime);
    void Render();
    void Release();

private:
    friend class TSingleton<GUI_Chat>;
    GUI_Chat()  = default;
    ~GUI_Chat() = default;

public:
    GUI_Chat(const GUI_Chat&)            = delete;
    GUI_Chat& operator=(const GUI_Chat&) = delete;
};
