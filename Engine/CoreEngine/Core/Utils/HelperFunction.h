#pragma once
#include <filesystem>

inline std::string ParseFile(const std::string& InFile)
{
	// Check if the InName is a path
	if (std::filesystem::path filePath(InFile); filePath.has_parent_path())
	{
		// If so, return the file name only
		if (filePath.has_filename())
		{
			return filePath.stem().string();
		}
		//raise an error or return an empty string
		return std::string();
	}

	// If not, return it as is
	return InFile;

}


inline std::wstring ParseFile(const std::wstring& InFile)
{
	// Check if the InName is a path
	if (std::filesystem::path filePath(InFile); filePath.has_parent_path())
	{
		// If so, return the file name only
		if (filePath.has_filename())
		{
			return filePath.stem().wstring();
		}
		//raise an error or return an empty string
		return std::wstring();
	}

	// If not, return it as is
	return InFile;

}
