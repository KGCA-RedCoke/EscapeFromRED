#include "JLevel_Intro.h"

JUIComponent* JLevel_Intro::GetUIComponent(uint32_t Index) const
{
	if (Index < mUIComponents.size())
	{
		return mUIComponents[Index];
	}
	return nullptr;
}
