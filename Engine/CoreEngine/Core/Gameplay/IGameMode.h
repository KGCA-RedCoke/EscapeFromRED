#pragma once
#include "Core/Utils/FileIO/JSerialization.h"

/**
 * 게임 플레이 관리 클래스
 * 게임에 대한 전반적인 플레이를 관리
 * * 게임 시작 (플레이어 생성, 레벨 로드)
 * * 종료
 * * 플레이어 생성
 * * 게임 오버 등을 관리
 */
class IGameMode : public JAsset
{
public:
	~IGameMode() override = default;
	/**
	 * 게임 시작
	 */
	virtual void StartGame() = 0;

	/**
	 * 게임 종료
	 */
	virtual void EndGame() = 0;

	/**
	 * 플레이어 생성
	 */
	virtual void CreatePlayer() = 0;

	/**
	 * 게임 오버
	 */
	virtual void GameOver() = 0;

	virtual bool IsInGame() const = 0;
};

class JGameMode : public IGameMode
{
public:
	JGameMode()           = default;
	~JGameMode() override = default;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void StartGame() override;
	void EndGame() override;
	void CreatePlayer() override;
	void GameOver() override;
	bool IsInGame() const override { return bInGame; }

private:
	bool bInGame = false;

};
