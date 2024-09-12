// #pragma once
//
// #include <Xinput.h>
//
// constexpr float InputDeadZone = 0.24f * static_cast<FLOAT>(0X7FFF);
//
// struct FControllerState
// {
// 	bool             bConnected;
// 	XINPUT_STATE     PrevState;
// 	XINPUT_STATE     State;
// 	XINPUT_VIBRATION Vibration;
// };
//
// class XController
// {
// public:
// 	void Update(float DeltaTime);
//
// private:
// 	void CheckConnected();
//
// private:
// 	FControllerState mController;
// };
