#include "ResourcePaths.h"

#include <stdexcept>

namespace Jacob::Platform
{
namespace
{
bool IsDataRoot(const std::filesystem::path& path)
{
    return std::filesystem::is_regular_file(path / "Configs" / "engine.ini") &&
           std::filesystem::is_directory(path / "Shader");
}

std::filesystem::path Search(std::filesystem::path directory)
{
    if (directory.empty()) return {};
    directory = std::filesystem::absolute(directory).lexically_normal();
    for (;;)
    {
        if (IsDataRoot(directory)) return std::filesystem::canonical(directory);
        const auto sourceRoot = directory / "Engine" / "CoreEngine";
        if (IsDataRoot(sourceRoot)) return std::filesystem::canonical(sourceRoot);
        const auto parent = directory.parent_path();
        if (parent == directory) return {};
        directory = parent;
    }
}
}

std::filesystem::path ResolveDataRoot(const std::filesystem::path& explicitRoot,
                                      const std::filesystem::path& workingDirectory,
                                      const std::filesystem::path& executableDirectory)
{
    if (!explicitRoot.empty())
    {
        const auto root = explicitRoot.is_absolute() ? explicitRoot : workingDirectory / explicitRoot;
        if (!IsDataRoot(root))
            throw std::runtime_error("Invalid --data-root: Configs/engine.ini and Shader are required.");
        return std::filesystem::canonical(root);
    }
    if (auto root = Search(workingDirectory); !root.empty()) return root;
    if (auto root = Search(executableDirectory); !root.empty()) return root;
    throw std::runtime_error("Engine resources not found. Pass --data-root <Engine/CoreEngine directory>.");
}
}
