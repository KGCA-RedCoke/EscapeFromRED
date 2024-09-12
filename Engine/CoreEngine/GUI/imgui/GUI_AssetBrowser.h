#pragma once
#include "common_include.h"
#include "GUI_Base.h"
#include "Core/Graphics/Texture/MTextureManager.h"
// #include "Core/Texture/MTextureManager.h"

class JTexture;
namespace fs = std::filesystem;

enum class EFileType : uint8_t
{
	Folder = 0,
	Asset  = 1 << 0
};

/**
* \struct AssetSelectionWithDeletion
 *
 * 간략한 에셋 선택 및 삭제를 처리하는 구조체.
 *
 * 에셋 선택 및 삭제를 위한 기능을 제공하기 위해 ImGuiSelectionBasicStorage 구조체를 상속.
 * 여기에는 루프 전후에 삭제를 적용하고 선택 항목을 업데이트하는 메서드가 포함.
 */
struct AssetSelectionWithDeletion : ImGuiSelectionBasicStorage
{
	int ApplyDeletionPreLoop(ImGuiMultiSelectIO* MultiSelectIo, int32_t ItemsCount)
	{
		if (Size == 0)
			return -1;

		const int32_t focusedIndex = static_cast<int32_t>(MultiSelectIo->NavIdItem);
		if (!MultiSelectIo->NavIdSelected)
		{
			MultiSelectIo->RangeSrcReset = true;
			return focusedIndex;
		}

		for (int32_t index = focusedIndex + 1; index < ItemsCount; ++index)
		{
			if (!Contains(GetStorageIdFromIndex(index)))
				return index;
		}

		for (int32_t index = min(focusedIndex, ItemsCount) - 1; index >= 0; --index)
		{
			if (!Contains(GetStorageIdFromIndex(index)))
				return index;
		}

		return -1;
	}

	template <typename ItemType>
	void ApplyDeletionPostLoop(ImGuiMultiSelectIO* MultiSelectIo, std::vector<ItemType>& Items,
							   int                 CurrentItemIndexToSelect)
	{
		// Rewrite item list (delete items) + convert old selection index (before deletion) to new selection index (after selection).
		// If NavId was not part of selection, we will stay on same item.
		std::vector<ItemType> new_items;
		new_items.reserve(Items.size() - Size);
		int item_next_idx_to_select = -1;
		for (int idx = 0; idx < Items.size(); idx++)
		{
			if (!Contains(GetStorageIdFromIndex(idx)))
				new_items.push_back(Items[idx]);
			if (CurrentItemIndexToSelect == idx)
				item_next_idx_to_select = new_items.size() - 1;
		}
		Items.swap(new_items);

		// Update selection
		Clear();
		if (item_next_idx_to_select != -1 && MultiSelectIo->NavIdSelected)
			SetItemSelected(GetStorageIdFromIndex(item_next_idx_to_select), true);
	}
};

struct AssetBrowserIconList
{

	void CreateTextures()
	{
		FolderIcon = Manager_Texture.CreateOrLoad(L"rsc/Icons/Folders/Folder_Base_64x.png");
		FileIcon   = Manager_Texture.CreateOrLoad(L"rsc/Icons/AssetIcons/Actor_64x.png");
	}

public
:
	JTexture* FolderIcon;
	JTexture* FileIcon;
};

struct FBasicFilePreview
{
	uint32_t  ID; // Hash값 (path로 지정)
	JWText    FileName;
	fs::path  FilePath;
	EFileType FileType;
};


class GUI_AssetBrowser : public GUI_Base
{
public:
	GUI_AssetBrowser(const std::string& InTitle);
	~GUI_AssetBrowser() override = default;

public:
	void Initialize() override;
	void Update(float_t DeltaTime) override;
	void Release() override;
	void UpdateMultiSelection(ImVec2 start_pos);

private:
	void SetWindowSize(int32_t InWidth, int32_t InHeight) const;
	void SetMultiFlagOptions();
	void UpdateLayoutSizes(float InAvailWidth);
	void UpdateClipperAndItemSpacing(ImGuiMultiSelectIO* msIO, ImVec2 startPos, int currentItemIndexToFocus);
	void UpdateDragDrop(bool bIsItemSelected, ImGuiID payLoadID);
	void UpdateIcon(ImVec2 pos, int bIsItemSelected, FBasicFilePreview* itemData) const;
	void UpdateZoom(ImVec2 startPos, float availableWidth);

	void HandleFile();

private:
	float mIconHitSpacing;
	float mZoomWheelAccum;

	// ------------ layout --------------
	float  mLayoutOuterPadding;
	float  mLayoutItemSpacing;
	float  mLayoutSelectableSpacing;
	float  mIconSpacing;
	ImVec2 mLayoutItemSize;
	ImVec2 mLayoutItemStep;
	int    mLayoutColumnCount;
	int    mLayoutLineCount;
	// ------------ layout --------------

	// ----------- settings -------------
	int32_t               mIconSize;
	ImGuiMultiSelectFlags mMultiSelectFlag;
	bool                  bOpen;
	bool                  bAllowBoxSelect;
	bool                  bAllowDragUnselected;
	bool                  bRequestDelete;
	bool                  bRequestRename;
	bool                  bShowTypeOverlay;
	bool                  bStretchSpacing;
	// ----------- settings -------------

	// ------------- data ---------------
	AssetSelectionWithDeletion     mSelection;
	std::vector<FBasicFilePreview> mFiles;
	JWText                         mCurrentDirectory; // 현재 폴더 경로
	JWText                         mCachedDirectory;  // 이전 폴더 경로
	// ------------- data ---------------

	JWText mTempRenameText;

	AssetBrowserIconList g_IconList;
};
