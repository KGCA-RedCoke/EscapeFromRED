#include "GUI_AssetBrowser.h"

#include "Core/Utils/Utils.h"
#include "Core/Utils/Math/MathUtility.h"


GUI_AssetBrowser::GUI_AssetBrowser(const std::string& InTitle)
	: GUI_Base(InTitle),
	  mIconHitSpacing(4),
	  mZoomWheelAccum(0),
	  mLayoutOuterPadding(0),
	  mLayoutItemSpacing(),
	  mLayoutSelectableSpacing(0),
	  mIconSpacing(10),
	  mLayoutColumnCount(0),
	  mLayoutLineCount(0),
	  mIconSize(64),
	  mMultiSelectFlag(0),
	  bOpen(false),
	  bAllowBoxSelect(true),
	  bAllowDragUnselected(false),
	  bRequestDelete(false),
	  bRequestRename(false),
	  bShowTypeOverlay(true),
	  bStretchSpacing(true)
{
	mCurrentDirectory = fs::absolute(L"Game");
}


void GUI_AssetBrowser::Initialize()
{
	GUI_Base::Initialize();

	g_IconList.FileIcon   = IManager.TextureManager->CreateOrLoad(L"rsc/Icons/AssetIcons/Actor_64x.png");
	g_IconList.FolderIcon = IManager.TextureManager->CreateOrLoad(L"rsc/Icons/Folders/Folder_BaseHi_256x.png");

	SetMultiFlagOptions();
}

void GUI_AssetBrowser::Update(float DeltaTime)
{
	// 파일 목록 업데이트 (디렉토리 변경 시)
	HandleFile();

	// 파일 브라우저 창 크기 설정 (Desired Size)
	SetWindowSize(10, 15);

	if (!ImGui::Begin(mTitle.c_str(), &bOpen, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	// 창이 Docked 되어있지 않다면 스타일 변경
	ChangeWindowStyleIfNotDocked();

	/// 메뉴바를 생성해준다.
	/// 메뉴바에는 파일 불러오기, Fbx파일 파싱해서 jasset형식으로 저장하기 등이 있을 수 있다.
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open"))
			{
				// 파일 불러오기 (파일 선택창 띄우기)
				// 현재 파일 위치는 기본적으로 Game 폴더로 설정되어있음
				// if (const auto path = OpenFile(L"Game", L"JEngine Asset (*.jasset)\0*.jasset\0"))
				// {
				// 	// 파일 불러오기
				// }
			}
			if (ImGui::MenuItem("Save"))
			{
				// 파일 저장
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// 창 내부 크기 설정
	ImGui::SetNextWindowContentSize(ImVec2(0.0f,
										   mLayoutOuterPadding + mLayoutLineCount *
										   (mLayoutItemSize.x + mLayoutItemSpacing)));

	// 창 내부에 독립적인 레이아웃(스크롤)을 가지는 UI 생성
	if (ImGui::BeginChild("Assets",
						  ImVec2(0.0f, -ImGui::GetTextLineHeightWithSpacing()),
						  ImGuiChildFlags_Border,
						  ImGuiWindowFlags_NoMove))
	{

		// 창에서 남아있는 사용가능한 영역 확인
		const float availableWidth = ImGui::GetContentRegionAvail().x;
		UpdateLayoutSizes(availableWidth);

		// 스크린(전체화면)에서의 현재 UI 커서 위치에 패딩을 더해준다. (내부 그리기 시작 위치 지정)
		ImVec2 startPos = ImGui::GetCursorScreenPos();
		startPos        = ImVec2(startPos.x + mLayoutOuterPadding, startPos.y + mLayoutOuterPadding);
		ImGui::SetCursorScreenPos(startPos);

		UpdateMultiSelection(startPos);

		UpdateZoom(startPos, availableWidth);

		ImGui::EndChild();
	}

	ImGui::Text("Selected: %d/%d items \t", mSelection.Size, mFiles.size());
	ImGui::SameLine();

	ImGui::Text("Current Path: %s", WString2String(mCurrentDirectory).c_str());

	ImGui::SameLine();
	UpdateProgressBar();

	ImGui::End();
}

void GUI_AssetBrowser::Release()
{
	GUI_Base::Release();
}

void GUI_AssetBrowser::SetWindowSize(int32_t InWidth, int32_t InHeight) const
{
	// 창 외부 크기 설정
	ImGui::SetNextWindowSize(
							 ImVec2(
									mIconSize * InWidth,
									mIconSize * InHeight),
							 ImGuiCond_FirstUseEver // 이 창에 대한 사전 설정이 없을 때만 
							);
}

void GUI_AssetBrowser::UpdateMultiSelection(ImVec2 start_pos)
{
	ImGuiMultiSelectIO* msIO =
			ImGui::BeginMultiSelect(
									mMultiSelectFlag,
									mSelection.Size, // [option] esc clear되는 flag같은 걸 넣었을때 이 값이 0이라면 비용을 아낄 수 있다
									mFiles.size() // [option] 아직 잘 모르겠다...
								   );

	// Use custom selection adapter: store ID in selection (recommended)
	mSelection.UserData                = this;
	mSelection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self_, int idx){
		GUI_AssetBrowser* self = static_cast<GUI_AssetBrowser*>(self_->UserData);
		return self->mFiles[idx].ID;
	};
	mSelection.ApplyRequests(msIO);

	const bool bWantDelete =
			(ImGui::Shortcut(ImGuiKey_Delete, ImGuiInputFlags_Repeat) && (mSelection.Size > 0)) || bRequestDelete;
	const bool bWantRename =
			(ImGui::Shortcut(ImGuiKey_F2, ImGuiInputFlags_Repeat) && mSelection.Size > 0) || bRequestRename;

	const int currentItemIndexToFocus =
			bWantDelete || bWantRename ? mSelection.ApplyDeletionPreLoop(msIO, mFiles.size()) : -1;

	UpdateClipperAndItemSpacing(msIO, start_pos, currentItemIndexToFocus);

	// Context menu (오른쪽 클릭)
	if (ImGui::BeginPopupContextWindow())
	{
		if (mSelection.Size > 0)
		{
			ImGui::Text("mSelection: %d items", mSelection.Size);
			ImGui::Separator();
			if (ImGui::MenuItem("Delete", "Del", false, mSelection.Size > 0))
			{
				bRequestDelete = true;
			}
			if (ImGui::MenuItem("Rename", "F2", false, mSelection.Size == 1))
			{
				bRequestRename = true;
			}
		}

		else
		{
			if (ImGui::MenuItem("New Folder"))
			{
				// 코드를 추가하여 새 폴더 생성을 처리

			}
			if (ImGui::MenuItem("New File"))
			{
				// 코드를 추가하여 새 파일 생성을 처리
			}
		}
		ImGui::EndPopup();

	}

	msIO = ImGui::EndMultiSelect();
	mSelection.ApplyRequests(msIO);
	if (bWantDelete)
		mSelection.ApplyDeletionPostLoop(msIO, mFiles, currentItemIndexToFocus);
}

void GUI_AssetBrowser::SetMultiFlagOptions()
{
	// Flag에 대한 부분은 imgui doc 참고(wip...)
	// Multi-select	Flag										ESC		 |  empty location 누를시에 Clear
	mMultiSelectFlag = ImGuiMultiSelectFlags_ClearOnEscape | ImGuiMultiSelectFlags_ClearOnClickVoid;

	// - Enable box-select (in 2D mode, so that changing box-select rectangle X1/X2 boundaries will affect clipped items)
	if (bAllowBoxSelect)
		mMultiSelectFlag |= ImGuiMultiSelectFlags_BoxSelect1d;

	// - This feature allows dragging an unselected item without selecting it (rarely used)
	if (bAllowDragUnselected)
		mMultiSelectFlag |= ImGuiMultiSelectFlags_SelectOnClickRelease;

	// - Enable keyboard wrapping on X axis
	// (FIXME-MULTISELECT: We haven't designed/exposed a general nav wrapping api yet, so this flag is provided as a courtesy to avoid doing:
	//    ImGui::NavMoveRequestTryWrapping(ImGui::GetCurrentWindow(), ImGuiNavMoveFlags_WrapX);
	// When we finish implementing a more general API for this, we will obsolete this flag in favor of the new system)
	mMultiSelectFlag |= ImGuiMultiSelectFlags_NavWrapX;
}

void GUI_AssetBrowser::UpdateLayoutSizes(float InAvailWidth)
{
	mLayoutItemSpacing = mIconSpacing;
	if (!bStretchSpacing)
	{
		InAvailWidth += floorf(mLayoutItemSpacing * 0.5f);
	}

	mLayoutItemSize    = ImVec2(mIconSize, mIconSize);
	mLayoutColumnCount = max((int)(InAvailWidth / (mLayoutItemSize.x + mLayoutItemSpacing)), 1);
	mLayoutLineCount   = (mFiles.size() + mLayoutColumnCount - 1) / mLayoutColumnCount;

	if (bStretchSpacing && mLayoutColumnCount > 1)
		mLayoutItemSpacing = floorf(InAvailWidth - mLayoutItemSize.x * mLayoutColumnCount) / mLayoutColumnCount;

	mLayoutItemStep          = ImVec2(mLayoutItemSize.x + mLayoutItemSpacing, mLayoutItemSize.y + mLayoutItemSpacing);
	mLayoutSelectableSpacing = max(floorf(mLayoutItemSpacing) - mIconHitSpacing, 0.0f);
	mLayoutOuterPadding      = floorf(mLayoutItemSpacing * 0.5f);
}

void GUI_AssetBrowser::UpdateClipperAndItemSpacing(ImGuiMultiSelectIO* msIO, ImVec2 startPos, int currentItemIndexToFocus)
{
	bRequestDelete = false;

	// Push LayoutSelectableSpacing (which is LayoutItemSpacing minus hit-spacing, if we decide to have hit gaps between items)
	// Altering style ItemSpacing may seem unnecessary as we position every items using SetCursorScreenPos()...
	// But it is necessary for two reasons:
	// - Selectables uses it by default to visually fill the space between two items.
	// - The vertical spacing would be measured by Clipper to calculate line height if we didn't provide it explicitly (here we do).
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(mLayoutSelectableSpacing, mLayoutSelectableSpacing));

	const int columnCount = mLayoutColumnCount;

	// 대량의 항목을 렌더할 때 보이지 않는 부분들은 자동으로 제외시키는 ClipperList
	ImGuiListClipper clipper;

	clipper.Begin(mLayoutLineCount, mLayoutItemStep.y);

	if (currentItemIndexToFocus != -1)
		clipper.IncludeItemByIndex(currentItemIndexToFocus / columnCount); // Ensure focused item line is not clipped.
	if (msIO->RangeSrcItem != -1)
		clipper.
				IncludeItemByIndex(static_cast<int>(msIO->RangeSrcItem) /
								   columnCount); // Ensure RangeSrc item line is not clipped.

	// 범위 내 렌더
	while (clipper.Step())
	{
		for (int lineIndex = clipper.DisplayStart; lineIndex < clipper.DisplayEnd; lineIndex++)
		{
			// 라인 시작 인덱스 (한줄에 10개씩이면 0, 11, 21 ...)
			const int itemLineIndex = lineIndex * columnCount;
			// 라인 끝 인덱스 (10, 20, 30 ...)
			const int itemLineIndexEnd = min((lineIndex + 1) * columnCount, mFiles.size());

			// 한 줄씩 렌더링 시작
			for (int itemIndex = itemLineIndex; itemIndex < itemLineIndexEnd; ++itemIndex)
			{
				auto* itemData = &mFiles[itemIndex];

				// 특정 ID에 대한 아이템 처리 시작
				ImGui::PushID(itemData->FileName.c_str());

				// Set Position item
				ImVec2 pos = ImVec2(startPos.x + (itemIndex % columnCount) * mLayoutItemStep.x,
									startPos.y + lineIndex * mLayoutItemStep.y);
				ImGui::SetCursorScreenPos(pos);

				// Visualize 업데이트

				ImGui::SetNextItemSelectionUserData(itemIndex);
				bool bIsItemSelected = mSelection.Contains(itemData->ID);
				ImGui::Selectable("", bIsItemSelected, ImGuiSelectableFlags_None, mLayoutItemSize);

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				{
					switch (itemData->FileType)
					{
					case EFileType::Folder: // 폴더 업데이트
						mCurrentDirectory = itemData->FilePath;
						break;
					case EFileType::Asset: // 에셋 창 열기(너무 복잡, 그냥 DragDrop만 가능하도록 수정)
						
						break;
					}
				}

				// Update our selection state immediately (without waiting for EndMultiSelect() requests)
				// because we use this to alter the color of our text/icon.
				if (ImGui::IsItemToggledSelection())
					bIsItemSelected = !bIsItemSelected;

				// Focus (for after deletion)
				if (currentItemIndexToFocus == itemIndex)
					ImGui::SetKeyboardFocusHere(-1);

				// Drag and drop
				UpdateDragDrop(bIsItemSelected, itemData->FilePath.string());

				// Icon
				UpdateIcon(pos, bIsItemSelected, itemData);

				ImGui::PopID();
			}
		}
	}
	clipper.End();
	ImGui::PopStyleVar(); // ImGuiStyleVar_ItemSpacing
}

void GUI_AssetBrowser::UpdateDragDrop(bool bIsItemSelected, JText ItemPath)
{
	if (ImGui::BeginDragDropSource())
	{
		// Create payload with full selection OR single unselected item.
		// (the later is only possible when using ImGuiMultiSelectFlags_SelectOnClickRelease)
		if (ImGui::GetDragDropPayload() == NULL)
		{
			ImVector<JText> payload_items;
			void*           it = NULL;
			ImGuiID         id = 0;
			payload_items.push_back(ItemPath);
			// if (!bIsItemSelected)
			// 	payload_items.push_back(ItemPath);
			// else
			// 	while (mSelection.GetNextSelectedItem(&it, &id))
			// 		payload_items.push_back(id);
			ImGui::SetDragDropPayload("ASSETS_BROWSER_ITEMS",
									  ItemPath.c_str(),
									  ItemPath.size() + 1);
		}

		// Display payload content in tooltip, by extracting it from the payload data
		// (we could read from selection, but it is more correct and reusable to read from payload)
		const ImGuiPayload* payload       = ImGui::GetDragDropPayload();
		const int           payload_count = (int)payload->DataSize / (int)sizeof(JText);
		ImGui::Text("%d assets", payload_count);

		ImGui::EndDragDropSource();
	}
}

void GUI_AssetBrowser::UpdateIcon(ImVec2 pos, int bIsItemSelected, FBasicFilePreview* itemData) const
{
	// Rendering parameters
	bool bItemIsVisible = ImGui::IsRectVisible(mLayoutItemSize);

	const ImU32 iconBgColor   = ImGui::GetColorU32(ImGuiCol_MenuBarBg);
	const bool  bDisplayLabel = (mLayoutItemSize.x >= ImGui::CalcTextSize("999").x);

	// Render icon (a real app would likely display an image/thumbnail here)
	// Because we use ImGuiMultiSelectFlags_BoxSelect2d, clipping vertical may occasionally be larger, so we coarse-clip our rendering as well.
	if (bItemIsVisible)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImVec2 box_min(pos.x - 1, pos.y - 1);
		ImVec2 box_max(box_min.x + mLayoutItemSize.x + 2, box_min.y + mLayoutItemSize.y + 2); // Dubious
		draw_list->AddRectFilled(box_min, box_max, iconBgColor); // Background color

		// if (itemData->GetType() != 0)
		{
			draw_list->AddImage(
								itemData->FileType == EFileType::Folder
									? g_IconList.FolderIcon->GetSRV()
									: g_IconList.FileIcon->GetSRV(),
								box_min + ImVec2(2, 2),
								box_max - ImVec2(2, 2));
		}

		// if (bShowTypeOverlay /*&& item_data->GetType() != 0*/)
		// {
		// 	ImU32 type_col = icon_type_overlay_colors[itemData->GetType() %
		// 		IM_ARRAYSIZE(icon_type_overlay_colors)];
		// 	draw_list->AddRectFilled(ImVec2(box_max.x - 2 - icon_type_overlay_size.x, box_min.y + 2),
		// 							 ImVec2(box_max.x - 2, box_min.y + 2 + icon_type_overlay_size.y),
		// 							 type_col);
		// }
		if (bDisplayLabel)
		{
			if (bRequestRename && bIsItemSelected)
			{
				char inputText[256];
				ImGui::InputText("", inputText, 256);
			}
			else
			{
				ImU32 label_col = ImGui::GetColorU32(bIsItemSelected ? ImGuiCol_Text : ImGuiCol_TextDisabled);
				draw_list->AddText(ImVec2(box_min.x, box_max.y - ImGui::GetFontSize()),
								   label_col,
								   WString2String(itemData->FileName).c_str());
			}

		}
	}

}

void GUI_AssetBrowser::UpdateZoom(ImVec2 startPos, float availableWidth)
{
	// 렌더링 중? Skip
	if (ImGui::IsWindowAppearing())
		mZoomWheelAccum = 0.0f;

	ImGuiIO& io = ImGui::GetIO();
	// Zooming with CTRL+Wheel
	if (ImGui::IsWindowHovered() &&
		io.MouseWheel != 0.0f &&
		ImGui::IsKeyDown(ImGuiMod_Ctrl) &&
		!ImGui::IsAnyItemActive() // 여기서 Item은 사용자입력(Input)에 반응하는 Item
	)
	{
		mZoomWheelAccum += io.MouseWheel;
		if (fabsf(mZoomWheelAccum) >= 1.0f)
		{
			// Calculate hovered item index from mouse location
			// FIXME: Locking aiming on 'hovered_item_idx' (with a cool-down timer) would ensure zoom keeps on it.
			const float hoveredItemX     = (io.MousePos.x - startPos.x + mLayoutItemSpacing * 0.5f) / mLayoutItemStep.x;
			const float hoveredItemY     = (io.MousePos.y - startPos.y + mLayoutItemSpacing * 0.5f) / mLayoutItemStep.y;
			const int   hoveredItemIndex = (static_cast<int>(hoveredItemY) * mLayoutColumnCount) + static_cast<int>(
				hoveredItemX);
			//ImGui::SetTooltip("%f,%f -> item %d", hovered_item_nx, hovered_item_ny, hovered_item_idx); // Move those 4 lines in block above for easy debugging

			// Zoom
			mIconSize *= powf(1.1f, mZoomWheelAccum);
			mIconSize = FMath::Clamp(mIconSize, 16, 128);
			mZoomWheelAccum -= static_cast<int>(mZoomWheelAccum);
			UpdateLayoutSizes(availableWidth);

			// Manipulate scroll to that we will land at the same Y location of currently hovered item.
			// - Calculate next frame position of item under mouse
			// - Set new scroll position to be used in next ImGui::BeginChild() call.
			float hovered_item_rel_pos_y = (static_cast<float>(hoveredItemIndex / mLayoutColumnCount) +
				fmodf(hoveredItemY, 1.0f)) * mLayoutItemStep.y;
			hovered_item_rel_pos_y += ImGui::GetStyle().WindowPadding.y;
			float mouse_local_y = io.MousePos.y - ImGui::GetWindowPos().y;
			ImGui::SetScrollY(hovered_item_rel_pos_y - mouse_local_y);
		}
	}
}

void GUI_AssetBrowser::UpdateProgressBar() const
{
	ImGui::ProgressBar(mCurrentProgress, ImVec2(0, 0));
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	// ImGui::Text("TODO: 프로그레스 바 업데이트");
}

void GUI_AssetBrowser::HandleFile()
{
	// 디렉토리가 변경되었을 때만 업데이트
	if (mCachedDirectory == mCurrentDirectory)
		return;

	// 파일 목록 초기화
	mFiles.clear();

	// 디렉토리가 존재할 때만 파일 목록을 업데이트
	if (!fs::exists(mCurrentDirectory))
	{
		LOG_CORE_ERROR("Asset Browser : Invalid Directory");
		return;
	}

	const fs::path currentPath = fs::path(mCurrentDirectory);
	// 상위 디렉토리로 이동하는 버튼 추가를 위해 상위 디렉토리 경로 추가
	if (currentPath.has_parent_path())
	{
		const fs::path parentPath = fs::absolute(mCurrentDirectory).parent_path();
		mFiles.emplace_back(StringHash(parentPath.c_str()), L"..", parentPath, EFileType::Folder);
	}

	mCachedDirectory = mCurrentDirectory;

	for (auto& dir : fs::directory_iterator(currentPath))
	{
		auto path = dir.path();

		if (fs::is_directory(dir))
		{
			mFiles.emplace_back(StringHash(path.c_str()), path.stem(), path, EFileType::Folder);
		}
		else if (!path.extension().empty() && path.extension() == ".jasset")
		{
			mFiles.emplace_back(StringHash(path.c_str()), path.stem(), path, EFileType::Asset);
		}
	}
}
