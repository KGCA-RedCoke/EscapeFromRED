#pragma once
#include "JLevel.h"
#include "Core/Manager/Manager_Base.h"

class MLevelManager : public Manager_Base<JLevel, MLevelManager>
{
public:
#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MLevelManager>;

	MLevelManager();
	~MLevelManager();

public:
	MLevelManager(const MLevelManager&)            = delete;
	MLevelManager& operator=(const MLevelManager&) = delete;

#pragma endregion
};
