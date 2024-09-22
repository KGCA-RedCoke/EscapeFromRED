// #include "XController.h"
//
// void XController::CheckConnected()
// {
// 	mController.PrevState  = mController.State;
// 	mController.bConnected = XInputGetState(0, &mController.State) == ERROR_SUCCESS ? true : false;
// }
//
// void XController::Update(float DeltaTime)
// {
// 	CheckConnected();
//
// 	if (mController.bConnected)
// 	{
// 		WORD buttons = mController.State.Gamepad.wButtons;
//
// 		// --------------------------------------------------------------------------------------------------------------
// 		if ((mController.State.Gamepad.sThumbLX < InputDeadZone &&
// 				mController.State.Gamepad.sThumbLX > -InputDeadZone) &&
// 			(mController.State.Gamepad.sThumbLY < InputDeadZone &&
// 				mController.State.Gamepad.sThumbLY > -InputDeadZone))
// 		{
// 			mController.State.Gamepad.sThumbLX = 0;
// 			mController.State.Gamepad.sThumbLY = 0;
// 		}
//
// 		if ((mController.State.Gamepad.sThumbRX < InputDeadZone &&
// 				mController.State.Gamepad.sThumbRX > -InputDeadZone) &&
// 			(mController.State.Gamepad.sThumbRY < InputDeadZone &&
// 				mController.State.Gamepad.sThumbRY > -InputDeadZone))
// 		{
// 			mController.State.Gamepad.sThumbRX = 0;
// 			mController.State.Gamepad.sThumbRY = 0;
// 		}
// 		// --------------------------------------------------------------------------------------------------------------
//
// 		XInputSetState(0, &mController.Vibration);
//
//
// 	}
// }
