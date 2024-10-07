#include "GUI_Popup_FileBrowser.h"
#include "Core/Utils/Math/MathUtility.h"
#include "ImGui/imgui.h"
#include "imgui/imgui_internal.h"

inline JText ImGuiStdStringReplace(const JText& InStr, const JText& InSearch, const JText& InRep)
{
	if (InSearch.empty())
		return InStr;

	JText  result    = InStr;
	size_t start_pos = 0;
	while ((start_pos = result.find(InSearch, start_pos)) != JText::npos)
	{
		result.replace(start_pos, InSearch.length(), InRep);
		start_pos += InRep.length();
	}
	return result;
}

inline JText ImGuiStdStringRemove(const JText& InStr, const JText& InSearch)
{
	return ImGuiStdStringReplace(InStr, InSearch, "");
}

JText ImGui::FileBrowser::GetLabel() const
{
	return "File Browser##" + mOriginalPath;
}

JText ImGui::FileBrowser::GetLocal(const JText& InPath)
{
	const auto relativePath = std::filesystem::relative(InPath);
	return InPath;
	// return ImGuiStdStringRemove(relativePath.string(), "..\\content");
}

JText ImGui::FileBrowser::GetEditedPath(const JText& InPath)
{
	if (InPath.starts_with("..\\"))
		return InPath.substr(3);
	return InPath;
}

bool ImGui::OpenFileBrowser(const JText& InPath, FileBrowserOption InOption, const std::set<JText>& InExt)
{

	if (!FileBrowser::Instance)
		FileBrowser::Instance = new FileBrowser();

	return FileBrowser::Instance->OpenInternal(InPath, InOption, InExt);
}

bool ImGui::FetchFileBrowserResult(const JText& InPath, JText& OutSelectedPath)
{
	if (FileBrowser::Instance)
	{
		if (FileBrowser::Instance->FetchInternal(InPath, OutSelectedPath))
		{
			// Delete instance when finished
			delete FileBrowser::Instance;
			FileBrowser::Instance = nullptr;
			return true;
		}
	}
	return false;
}

bool ImGui::FileBrowser::OpenInternal(const JText&           InPath, FileBrowserOption InOption,
									  const std::set<JText>& InExt)
{
	// 경로가 이전과 같으면 Skip
	if (mIsOpen)
		return mOriginalPath == InPath;

	// 경로가 디렉토리가 아니면 상위 디렉토리로 변경
	JText directory = InPath;
	if (!std::filesystem::is_directory(directory))
	{
		const auto dirPath = std::filesystem::path(directory);
		if (dirPath.has_parent_path())
			directory = dirPath.parent_path().string();
	}

	// Setup
	mIsOpen       = true;
	mOriginalPath = InPath;
	mOption       = InOption;
	mExt          = InExt;

	if (TryApplyPath(directory))
	{
		OpenPopup(GetLabel().c_str());
		return true;
	}

	return false;
}

bool ImGui::FileBrowser::FetchInternal(const JText& InPath, JText& OutSelectedPath)
{
	if (!mIsOpen)
		return false;

	if (mOriginalPath != InPath)
		return false;

	bool     result = false;
	ImGuiIO& io     = GetIO();
	SetNextWindowSize({mWidth, mHeight});
	SetNextWindowPos(io.DisplaySize * 0.5f, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	SetNextWindowFocus();
	if (BeginPopupModal(GetLabel().c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		EditNavigation();
		Spacing();
		Separator();
		Spacing();
		EditContent();
		Spacing();

		// Extension text in the bottom right
		JText ext;
		switch (mOption)
		{
		case FileBrowserOption::DIRECTORY:
			ext = "Directory";
			break;
		case FileBrowserOption::FILE:
			for (auto e : mExt)
			{
				if (!ext.empty())
					ext += ", ";
				ext += e;
			}
			break;
		}
		if (!ext.empty())
		{
			Text((" Ext: " + ext).c_str());
			SameLine();
		}

		// Select / Cancel buttons
		const ImGuiStyle style = GetStyle();
		constexpr ImVec2 buttonSize(100.f, 0.f);
		const float      widthNeeded = buttonSize.x + style.ItemSpacing.x + buttonSize.x;
		SetCursorPosX(GetCursorPosX() + GetContentRegionAvail().x - widthNeeded);
		if (Button("Cancel", buttonSize))
		{
			OutSelectedPath = mOriginalPath;
			result          = true;
		}
		SameLine();
		if (Button("Select", buttonSize))
		{
			switch (mOption)
			{
			case FileBrowserOption::DIRECTORY:
				OutSelectedPath = mPath;
				break;
			case FileBrowserOption::FILE:
				OutSelectedPath = mSelected.empty() ? mOriginalPath : mPath + "\\" + mSelected;
				break;
			}
			while (OutSelectedPath.starts_with('\\'))
				OutSelectedPath = OutSelectedPath.substr(1);
			result = true;
		}

		EndPopup();
	}

	return result;
}

void ImGui::FileBrowser::Refresh()
{
	// 1. 현재 경로가 존재하지 않으면 상위 경로로 이동
	const std::filesystem::path path(mPath);
	if (!std::filesystem::exists(path))
	{
		TryPopPath();
		return;
	}

	// 2. Store directory content
	mDirectories.clear();
	mFiles.clear();
	for (auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			mDirectories.push_back(entry.path().filename().string());
		}
		else if (mOption != FileBrowserOption::DIRECTORY)
		{
			// Filter extensions
			if (!mExt.empty())
			{
				if (!entry.path().has_extension())
					continue;
				
				JText ext =entry.path().extension().string();
				std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
				if (!mExt.contains(ext))
					continue;
			}
			mFiles.push_back(entry.path().filename().string());
		}
	}

	mSelected        = "";
	mRenameResult    = "";
	mNewEntry        = false;
	mNavigationGuess = "";

	// 3. Refresh hint
	RefreshGuess();
}

void ImGui::FileBrowser::RefreshGuess()
{
	JText                       editedPath = GetEditedPath(mEditedPath);
	const std::filesystem::path path(editedPath);
	if (!path.has_parent_path())
		return;
	if (!exists(path.parent_path()))
		return;

	int   matchC = 0;
	JText matchPath;
	for (auto& entry : std::filesystem::directory_iterator(path.parent_path()))
	{
		// Find content of parent path
		if (!entry.is_directory())
			continue;

		// Compare to EditedPath
		// Find the one with greatest match
		JText entryPath = GetLocal(entry.path().string());
		int   i         = 0;
		for (i = 0; i < FMath::Min(entryPath.length(), editedPath.length()); i++)
			if (entryPath[i] != editedPath[i])
				break;
		if (i > matchC)
		{
			matchPath = entryPath;
			matchC    = i;
		}
	}

	if (matchC > 0)
		mNavigationGuess = matchPath;
}

bool ImGui::FileBrowser::TryPopPath()
{
	// Pop until exists or at root
	std::filesystem::path path(mPath);
	while (!std::filesystem::exists(path) && path.has_parent_path())
		path = path.parent_path();
	return TryApplyPath(GetLocal(path.parent_path().string()));
}

bool ImGui::FileBrowser::TryApplyPath(const JText& InString)
{
	const std::filesystem::path path(InString);
	if (!std::filesystem::exists(path))
		return false;

	mPath       = GetLocal(path.string());
	mEditedPath = "..\\" + mPath;
	Refresh();
	return true;
}

void ImGui::FileBrowser::EditNavigation()
{
	constexpr ImGuiInputTextFlags flags =
			ImGuiInputTextFlags_CharsNoBlank |
			ImGuiInputTextFlags_AutoSelectAll |
			ImGuiInputTextFlags_EnterReturnsTrue |
			ImGuiInputTextFlags_CallbackCompletion |
			ImGuiInputTextFlags_CallbackHistory;
	if (InputText("##Path", &mEditedPath[0], flags))
	{
		mEditedPath == ".." ? TryPopPath() : TryApplyPath(GetEditedPath(mEditedPath));
	}
	if (IsItemEdited())
		RefreshGuess();
	if (IsItemActive())
	{
		if (IsKeyDown(ImGuiKey_Tab))
		{
			mEditedPath == ".." ? TryPopPath() : TryApplyPath(mNavigationGuess);
			ClearActiveID();
		}
	}

	SameLine();
	if (Button("^"))
		TryPopPath();
	SameLine();
	if (Button("Refresh"))
		Refresh();
}

void ImGui::FileBrowser::EditContent()
{
	constexpr ImVec2 buttonSize(100.f, 0.f);

	if (Button("New", buttonSize))
	{
		Refresh();

		// We're creating a new entry and
		// using the rename feature to select a name
		JText newFileName = "untitled";
		mNewEntry         = true;
		mSelected         = newFileName;
		mRenameResult     = newFileName;
	}

	bool disabled = mSelected.empty();
	if (disabled)
		BeginDisabled();

	SameLine();
	if (Button("Duplicate", buttonSize))
	{
		// Duplicate current file / directory
		JText duplicate = TryDuplicate(mSelected);
		if (!duplicate.empty())
		{
			JText prevSelected = mSelected;
			Refresh();
			mSelected = prevSelected;
		}
	}

	SameLine();
	if (Button("Rename", buttonSize))
	{
		// Rename current entry
		mRenameResult = mSelected;
	}

	SameLine();
	if (Button("Delete", buttonSize))
	{
		// Delete current file / directory
		if (TryDelete(mSelected))
			Refresh();
	}

	if (disabled)
		EndDisabled();


	const ImGuiStyle style = GetStyle();
	const ImVec2     size  = {
		mWidth - style.WindowPadding.x * 2.0f,
		mHeight - style.WindowPadding.y * 2.0f - 115.0f
	};
	if (BeginListBox("##FileBrowserContent", size))
	{
		Spacing();
		Spacing();

		if (!mPath.empty())
			if (Selectable("  ..##FileListBack"))
				TryPopPath();

		bool renaming = !mRenameResult.empty();

		JText newDir;
		for (auto& dir : mDirectories)
		{
			if (ContentEntry(dir, true))
			{
				if (dir == mSelected)
				{
					newDir = dir;
				}
				else
				{
					mSelected     = dir;
					mRenameResult = "";
				}
			}
		}

		for (auto& file : mFiles)
		{
			if (ContentEntry(file, false))
			{
				mSelected     = file;
				mRenameResult = "";
			}
		}

		if (mNewEntry)
			ContentEntry(mSelected, false);

		if (!newDir.empty())
			TryApplyPath(mPath + "\\" + newDir);

		if (renaming && mRenameResult.empty())
			Refresh();

		Spacing();
		Spacing();

		EndListBox();
	}
}

bool ImGui::FileBrowser::ContentEntry(const JText& InEntry, bool InIsDir)
{
	const bool selected = mSelected == InEntry;
	if (selected && !mRenameResult.empty())
	{
		constexpr ImGuiInputTextFlags flags =
				ImGuiInputTextFlags_EnterReturnsTrue |
				ImGuiInputTextFlags_EscapeClearsAll |
				ImGuiInputTextFlags_CharsNoBlank;
		if (InputText("##ListEntryRename", &mRenameResult[0], flags))
		{
			TryApplyRename(mSelected, mRenameResult);
			mRenameResult = "";
		}
		return false;
	}
	return Selectable(((InIsDir ? "- " : "  ") + InEntry + "##ListEntry").c_str(), selected);
}

bool ImGui::FileBrowser::TryApplyRename(const JText& InPreviousName, const JText& InNewName) const
{
	const std::filesystem::path newPath(mPath + "\\" + InNewName);
	const std::filesystem::path oldPath(mPath + "\\" + InPreviousName);

	if (std::filesystem::exists(newPath))
		return false;

	if (!std::filesystem::exists(oldPath))
	{
		// Do not rename, instead create file / directory
		if (newPath.has_extension())
		{
			// Create file
			// Utility::WriteFile(Path + "\\" + InNewName, "");
		}
		else
		{
			// Create dir
			std::filesystem::create_directory(newPath);
		}
	}
	else
	{
		std::filesystem::rename(oldPath, newPath);
	}

	return std::filesystem::exists(newPath);
}

bool ImGui::FileBrowser::TryDelete(const JText& InName) const
{
	const std::filesystem::path path(mPath + "\\" + InName);
	if (!std::filesystem::exists(path))
		return false;
	return std::filesystem::remove_all(path);
}

JText ImGui::FileBrowser::TryDuplicate(const JText& InName) const
{
	const std::filesystem::path path(mPath + "\\" + InName);
	if (!std::filesystem::exists(path))
		return {};
	int copyC = 0;
	while (copyC < 100)
	{
		copyC++;
		const JText                 newName = InName + "_" + std::to_string(copyC);
		const std::filesystem::path newPath(mPath + "\\" + newName);
		if (!std::filesystem::exists(newPath))
		{
			std::filesystem::copy(path, newPath);
			if (std::filesystem::exists(newPath))
				return newName;
			return {};
		}
	}
	return {};
}
