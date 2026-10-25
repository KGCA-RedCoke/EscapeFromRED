#include "AppDefines.h"

int main(int argc, char** argv)
{
	uint32_t maxFrames = 0;
	for (int i = 1; i < argc; ++i)
	{
		if (std::string_view(argv[i]) == "--smoke-test") maxFrames = 120;
	}
	FBasicWindowData windowData{HD_X, HD_Y, false, true};
	Application app(Name_Engine_Version, windowData);
	app.Run(maxFrames);

	return 0;
}
