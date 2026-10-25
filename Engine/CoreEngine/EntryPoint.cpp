#include "AppDefines.h"
#include "Platform/ResourcePaths.h"
#include "Platform/Window.h"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string_view>

// Native wide arguments preserve Windows paths containing non-ASCII characters.
int wmain(int argc, wchar_t** argv)
{
    try
    {
        uint32_t maxFrames = 0;
        std::filesystem::path dataRoot;
        for (int i = 1; i < argc; ++i)
        {
            const std::wstring_view argument(argv[i]);
            if (argument == L"--smoke-test") maxFrames = 120;
            else if (argument == L"--data-root" && i + 1 < argc) dataRoot = argv[++i];
            else throw std::invalid_argument("Usage: CoreEngine [--smoke-test] [--data-root <directory>]");
        }
        // Compatibility boundary: existing asset loaders still use relative paths.
        // Resolve once before any worker threads or managers are initialized.
        std::filesystem::current_path(Jacob::Platform::ResolveDataRoot(
            dataRoot, std::filesystem::current_path(), Jacob::Platform::ExecutableDirectory()));
        FBasicWindowData windowData{HD_X, HD_Y, false, true};
        Application app(Name_Engine_Version, windowData);
        app.Run(maxFrames);
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "Engine startup failed: " << error.what() << '\n';
        return 1;
    }
}
