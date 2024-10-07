#pragma once

#include "common_include.h"


/**
 * 출처 : https://github.com/haraldwer/ImGui-FileBrowser
 */
namespace ImGui
{
	enum class FileBrowserOption
	{
		DIRECTORY,
		FILE
	};

	class FileBrowser
	{
	public:
		FileBrowser() = default;
		bool OpenInternal(const JText& InPath, FileBrowserOption InOption, const std::set<std::string>& InExt);
		bool FetchInternal(const JText& InPath, JText& OutSelectedPath);

	private:
		JText        GetLabel() const;
		static JText GetLocal(const JText& InPath);
		static JText GetEditedPath(const JText& InPath);

		void Refresh();
		void RefreshGuess();

		bool TryPopPath();
		bool TryApplyPath(const JText& InString);

		void EditNavigation();
		void EditContent();

		bool  ContentEntry(const JText& InEntry, bool InIsDir);
		bool  TryApplyRename(const JText& InPreviousName, const JText& InNewName) const;
		bool  TryDelete(const JText& InName) const;
		JText TryDuplicate(const JText& InName) const;

	public:
		inline static FileBrowser* Instance = nullptr;

	private:
		bool              mIsOpen = false;
		JText             mOriginalPath;
		JText             mPath;
		JText             mNavigationGuess;
		JText             mEditedPath;
		JText             mSelected;
		JText             mRenameResult;
		bool              mNewEntry = false;
		std::set<JText>   mExt;
		FileBrowserOption mOption = FileBrowserOption::FILE;

		const float mWidth  = 500.0f;
		const float mHeight = 400.0f;

		std::vector<JText> mDirectories;
		std::vector<JText> mFiles;
	};

	bool OpenFileBrowser(const std::string&           InPath, FileBrowserOption InOption = FileBrowserOption::FILE,
						 const std::set<std::string>& InExt                              = {});
	bool FetchFileBrowserResult(const std::string& InPath, std::string& OutSelectedPath);
}
