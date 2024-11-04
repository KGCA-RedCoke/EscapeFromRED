#pragma once
#include "common_include.h"
#include "GUI_Base.h"

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
	void ApplyDeletionPostLoop(ImGuiMultiSelectIO* MultiSelectIo, JArray<ItemType>& Items,
							   const int32_t       CurrentItemIndexToSelect)
	{
		// Rewrite item list (delete items) + convert old selection index (before deletion) to new selection index (after selection).
		// If NavId was not part of selection, we will stay on same item.
		JArray<ItemType> newItems;	// 업데이트 될 아이템 리스트
		newItems.reserve(Items.size() - Size);	// 새로운 아이템 리스트의 크기를 (기존 아이템 리스트 크기 - 선택된 아이템(삭제할) 크기)로 설정

		int nextItemIndexToSelect = -1;	// 삭제된 아이템 다음에 선택될 아이템 인덱스

		// 다음 선택될 아이템 인덱스를 찾아보자. 일단 순회하자.
		for (int idx = 0; idx < Items.size(); idx++)
		{
			// 삭제되려고 선택된 아이템이 아니라면 새로운 아이템 리스트에 추가
			if (!Contains(GetStorageIdFromIndex(idx)))
			{
				newItems.push_back(Items[idx]);
			}
			// 아이템들은 모두 디렉터리 / 파일이므로 실제로 삭제해준다.
			else
			{
				switch (Items[idx].FileType)
				{
				case EFileType::Folder:
					fs::remove_all(Items[idx].FilePath);
					break;
				case EFileType::Asset:
					fs::remove(Items[idx].FilePath);
					break;
				default:
					break;
				}
			}

			// 삭제되려고 선택된 아이템이라면 다음 선택될 아이템 인덱스를 이전 인덱스 값으로 설정
			if (CurrentItemIndexToSelect == idx)
			{
				nextItemIndexToSelect = newItems.size() - 1;
			}
		}
		Items.swap(newItems);

		// Update selection
		Clear();
		if (nextItemIndexToSelect != -1 && MultiSelectIo->NavIdSelected)
			SetItemSelected(GetStorageIdFromIndex(nextItemIndexToSelect), true);
	}
};

struct AssetBrowserIconList
{
	Ptr<class JTexture> FolderIcon;
	Ptr<JTexture>       FileIcon;
	Ptr<JTexture>       StaticMeshIcon;
	Ptr<JTexture>       SkeletalMeshIcon;
	Ptr<JTexture>       MaterialIcon;
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
	void Update(float DeltaTime) override;
	void Release() override;
	void UpdateMultiSelection(ImVec2 start_pos);

private:
	void SetWindowSize(int32_t InWidth, int32_t InHeight) const;
	void SetMultiFlagOptions();
	void UpdateLayoutSizes(float InAvailWidth);
	void UpdateClipperAndItemSpacing(ImGuiMultiSelectIO* msIO, ImVec2 startPos, int currentItemIndexToFocus);
	void UpdateDragDrop(bool bIsItemSelected, const JText& ItemPath);
	void UpdateIcon(ImVec2 pos, int bIsItemSelected, FBasicFilePreview* itemData);
	void UpdateZoom(ImVec2 startPos, float availableWidth);
	void UpdateProgressBar() const;

	void HandleFile();
	void ParseAsset(FBasicFilePreview* ItemData);

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

	// ------------- data ---------------
	AssetSelectionWithDeletion     mSelection;
	std::vector<FBasicFilePreview> mFiles;
	JWText                         mCurrentDirectory; // 현재 폴더 경로
	JWText                         mCachedDirectory;  // 이전 폴더 경로

	// -------------- Progress --------------
	bool  bLoadProgress    = false;
	float mCurrentProgress = 0.f;

	JWText mTempRenameText;

	AssetBrowserIconList g_IconList;
};
