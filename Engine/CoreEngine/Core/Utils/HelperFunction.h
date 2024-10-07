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

/**
 * 파일 경로에서 파일 이름만 추출
 * @param InFile 확인 할 풀 경로
 * @return 파일 이름만 반환 (확장자 제외)
 */
inline std::wstring ParseFile(const std::wstring& InFile)
{
	// 입력 파일이 경로인지 확인
	if (std::filesystem::path filePath(InFile); filePath.has_parent_path())
	{
		// 그렇다면, 파일 이름만 반환 (확장자 제외)
		if (filePath.has_filename())
		{
			return filePath.stem().wstring();
		}

		// 오류를 발생시키거나 빈 문자열을 반환
		return std::wstring();
	}

	// 그렇지 않으면 그대로 반환
	return InFile;
}

