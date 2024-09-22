#pragma once

class ICoreInterface
{
public:
	virtual ~ICoreInterface() = default;

public:
#pragma region Core Interface
	virtual void Initialize() = 0;
	virtual void Update(float DeltaTime) = 0;
	virtual void Release() = 0;
#pragma endregion
};
