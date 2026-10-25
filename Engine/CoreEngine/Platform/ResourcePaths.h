#pragma once

#include <filesystem>

namespace Jacob::Platform
{
// Validate an explicit data root, or search cwd and executable ancestors for the
// source layout. Never silently accept an invalid explicit override.
std::filesystem::path ResolveDataRoot(const std::filesystem::path& explicitRoot,
                                      const std::filesystem::path& workingDirectory,
                                      const std::filesystem::path& executableDirectory);
}
