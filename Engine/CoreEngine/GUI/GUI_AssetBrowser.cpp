﻿#include "GUI_AssetBrowser.h"

#include "MGUIManager.h"
#include "Core/Entity/Level/MLevelManager.h"
#include "Core/Graphics/Mesh/MMeshManager.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Utils/Utils.h"
#include "Core/Utils/Math/MathUtility.h"
#include "Editor/GUI_Editor_Material.h"
#include "Editor/Actor/GUI_Editor_Actor.h"
#include "Editor/Animation/GUI_Editor_Animator.h"
#include "Editor/Mesh/GUI_Editor_Mesh.h"
#include "Editor/Mesh/GUI_Editor_SkeletalMesh.h"
#include "Editor/UI/GUI_Editor_UI.h"
#include "imgui/imgui_stdlib.h"

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
	  mIconSize(96),
	  mMultiSelectFlag(0),
	  bOpen(false),
	  bAllowBoxSelect(true),
	  bAllowDragUnselected(false),
	  bRequestDelete(false),
	  bRequestRename(false),
	  bShowTypeOverlay(true),
	  bStretchSpacing(true)
{
	mCurrentDirectory = fs::relative(L"Game");
	OnBrowserChange.Bind(std::bind(&GUI_AssetBrowser::HandleFile, this));
}


void GUI_AssetBrowser::Initialize()
{
	GUI_Base::Initialize();

	g_IconList.TextureIcon      = MTextureManager::Get().Load(L"rsc/Icons/Actor/Texture2D_64x.png");
	g_IconList.ActorIcon        = MTextureManager::Get().Load(L"rsc/Icons/Actor/Actor_64.png");
	g_IconList.FileIcon         = MTextureManager::Get().Load(L"rsc/Icons/Actor/Actor_64x.png");
	g_IconList.FolderIcon       = MTextureManager::Get().Load(L"rsc/Icons/Folders/Folder_BaseHi_256x.png");
	g_IconList.StaticMeshIcon   = MTextureManager::Get().Load(L"rsc/Icons/Actor/StaticMesh_64.png");
	g_IconList.SkeletalMeshIcon = MTextureManager::Get().Load(L"rsc/Icons/Actor/Skeleton_64x.png");
	g_IconList.MaterialIcon     = MTextureManager::Get().Load(L"rsc/Icons/Actor/MaterialInstanceActor_64x.png");
	g_IconList.LevelIcon        = MTextureManager::Get().Load(L"rsc/Icons/Actor/World_64.png");
	g_IconList.WidgetIcon       = MTextureManager::Get().Load(L"rsc/Icons/Actor/WidgetBlueprint_64x.png");
	g_IconList.SoundIcon        = MTextureManager::Get().Load(L"rsc/Icons/Actor/SoundCue_64x.png");
	g_IconList.AnimationIcon    = MTextureManager::Get().Load(L"rsc/Icons/Actor/AnimSequence_64.png");
	g_IconList.AnimatorIcon     = MTextureManager::Get().Load(L"rsc/Icons/Actor/AnimInstance_64.png");

	SetMultiFlagOptions();
}

void GUI_AssetBrowser::Update(float DeltaTime)
{
	// 파일 목록 업데이트 (디렉토리 변경 시)
	if (mCachedDirectory != mCurrentDirectory)
	{
		HandleFile();
	}


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

	mSearchBar.DrawSearchBar();

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

	ImGui::Text(u8("선택: %d/%d 개 \t"), mSelection.Size, mFiles.size());
	ImGui::SameLine();

	ImGui::Text(u8("현재 경로: %s"), WString2String(mCurrentDirectory).c_str());

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
	mFilteredItems.clear();
	for (auto& item : mFiles)
	{
		if (mSearchBar.Filter.PassFilter(WString2String(item.FileName).c_str()))
		{
			mFilteredItems.push_back(&item);
		}
	}

	ImGuiMultiSelectIO* msIO =
			ImGui::BeginMultiSelect(
									mMultiSelectFlag,
									mSelection.Size, // [option] esc clear되는 flag같은 걸 넣었을때 이 값이 0이라면 비용을 아낄 수 있다
									mFilteredItems.size() // [option] 아직 잘 모르겠다...
								   );

	// Use custom selection adapter: store PinID in selection (recommended)
	mSelection.UserData                = this;
	mSelection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self_, int idx){
		GUI_AssetBrowser* self = static_cast<GUI_AssetBrowser*>(self_->UserData);
		return self->mFilteredItems[idx]->ID;
	};
	mSelection.ApplyRequests(msIO);

	const bool bWantDelete =
			(ImGui::Shortcut(ImGuiKey_Delete, ImGuiInputFlags_Repeat) && (mSelection.Size > 0)) || bRequestDelete;
	const bool bWantRename =
			(ImGui::Shortcut(ImGuiKey_F2, ImGuiInputFlags_Repeat) && mSelection.Size > 0) || bRequestRename;

	const int currentItemIndexToFocus =
			bWantDelete /*|| bWantRename */ ? mSelection.ApplyDeletionPreLoop(msIO, mFilteredItems.size()) : -1;

	UpdateClipperAndItemSpacing(msIO, start_pos, currentItemIndexToFocus);

	if (bOpenActorPopup)
	{
		OpenInputPopup(mNewFileName);
	}

	// Context menu (오른쪽 클릭)
	if (ImGui::BeginPopupContextWindow())
	{
		if (mSelection.Size > 0)
		{
			ImGui::Text(u8("현재 선택 아이템: %d 개"), mSelection.Size);
			ImGui::Separator();
			if (ImGui::MenuItem(u8("삭제"), "Del", false, mSelection.Size > 0))
			{
				bRequestDelete = true;
				OnBrowserChange.Execute();
			}
			if (ImGui::MenuItem(u8("이름 변경"), "F2", false, mSelection.Size == 1))
			{
				bRequestRename = true;
				OnBrowserChange.Execute();
			}
		}

		else
		{
			if (ImGui::MenuItem(u8("새 폴더")))
			{
				// 코드를 추가하여 새 폴더 생성을 처리
				fs::create_directory(mCurrentDirectory + L"/NewFolder");

				OnBrowserChange.Execute();
			}
			if (ImGui::BeginMenu(u8("새 파일")))
			{
				JText newRelativePath = WString2String(mCurrentDirectory);
				JText newFileName;
				if (ImGui::MenuItem(u8("레벨 생성")))
				{
					newFileName = GenerateUniqueFileName(newRelativePath, "NewLevel", ".jasset");

					JLevel* newLevel        = MLevelManager::Get().CreateLevel(newFileName);
					newLevel->bThreadLoaded = true;

					if (Utils::Serialization::Serialize(newFileName.c_str(), newLevel))
					{
						OnBrowserChange.Execute();
					}
				}
				if (ImGui::MenuItem(u8("액터 생성")))
				{
					mNewFileName    = GenerateUniqueFileName(newRelativePath, "NewActor", ".jasset");
					bOpenActorPopup = true;
				}
				if (ImGui::MenuItem(u8("머티리얼 생성")))
				{
					newFileName = GenerateUniqueFileName(newRelativePath, "NewMaterial", ".jasset");
					if (auto ptr = MGUIManager::Get().Load<GUI_Editor_Material>(newFileName))
					{
						ptr->OpenIfNotOpened();
						OnBrowserChange.Execute();
					}
				}
				if (ImGui::MenuItem(u8("애니메이터 생성")))
				{
					newFileName = GenerateUniqueFileName(newRelativePath, "NewAnimator", ".jasset");
					if (auto ptr = MGUIManager::Get().Load<GUI_Editor_Animator>(newFileName))
					{
						ptr->OpenIfNotOpened();
						OnBrowserChange.Execute();
					}
				}
				ImGui::EndMenu();
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
			const int itemLineIndexEnd = min((lineIndex + 1) * columnCount, mFilteredItems.size());

			// int32_t filteredIndex = itemLineIndex;
			// 한 줄씩 렌더링 시작
			for (int itemIndex = itemLineIndex; itemIndex < itemLineIndexEnd; ++itemIndex)
			{
				auto itemData = mFilteredItems[itemIndex];


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
						HandleAssetClicked(itemData);
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

void GUI_AssetBrowser::UpdateDragDrop(bool bIsItemSelected, const JText& ItemPath)
{
	if (ImGui::BeginDragDropSource())
	{
		// Create payload with full selection OR single unselected item.
		// (the later is only possible when using ImGuiMultiSelectFlags_SelectOnClickRelease)
		if (ImGui::GetDragDropPayload() == NULL)
		{
			ImVector<JText> payload_items;
			payload_items.push_back(ItemPath);
			ImGui::SetDragDropPayload("ASSETS_BROWSER_ITEMS",
									  ItemPath.c_str(),
									  ItemPath.size() + 1);
		}

		// Display payload content in tooltip, by extracting it from the payload data
		// (we could read from selection, but it is more correct and reusable to read from payload)
		const ImGuiPayload* payload       = ImGui::GetDragDropPayload();
		const int           payload_count = static_cast<int>(payload->DataSize) / static_cast<int>(sizeof(JText));
		ImGui::Text("%d assets", payload_count);

		ImGui::EndDragDropSource();
	}
}

void GUI_AssetBrowser::UpdateIcon(ImVec2 pos, int bIsItemSelected, FBasicFilePreview* itemData)
{
	// Rendering parameters
	const bool bItemIsVisible = ImGui::IsRectVisible(mLayoutItemSize);
	const bool bDisplayLabel  = (mLayoutItemSize.x >= ImGui::CalcTextSize("999").x);

	// 자, 여기에 아이콘을 그리는 코드를 작성하면 된다.
	if (bItemIsVisible)
	{
		// ImGui에서 제공하는 DrawList는 PrimitiveType들을 그리는데에 도움을 받을 수 있다. (물론 ImGui창 내에서만)
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// Box 경계를 결정
		ImVec2 boxMin(pos.x - 1, pos.y - 1);
		ImVec2 boxMax(boxMin.x + mLayoutItemSize.x + 2, boxMin.y + mLayoutItemSize.y + 2); // Dubious

		// 아이콘 배경색
		const ImU32 iconBgColor = ImGui::GetColorU32(ImGuiCol_WindowBg);	// 아이콘 배경색은 창 배경색과 동일 (이게 더 깔끔)

		// 사각형으로 아이콘 배경을 그린다. 
		drawList->AddRectFilled(boxMin, ImVec2(boxMax.x, boxMax.y - 2), iconBgColor, 5);

		// 아이콘을 선택할 텍스처 포인터
		void* iconTexture = nullptr;

		// 파일 종류에 따라 아이콘 텍스처 선택
		JAssetMetaData metaData = Utils::Serialization::GetType(itemData->FilePath.string().c_str());

		iconTexture = [&, metaData, itemData]{
			if (itemData->FileType == EFileType::Folder)
				return g_IconList.FolderIcon->GetSRV();
			else if (itemData->FileType == EFileType::Texture)
				return GetWorld.TextureManager->Load(itemData->FilePath)->GetSRV();
			switch (metaData.AssetType)
			{
			case HASH_ASSET_TYPE_Actor:
				return g_IconList.ActorIcon->GetSRV();
			case HASH_ASSET_TYPE_STATIC_MESH:
				return g_IconList.StaticMeshIcon->GetSRV();
			case HASH_ASSET_TYPE_SKELETAL_MESH:
				return g_IconList.SkeletalMeshIcon->GetSRV();
			case HASH_ASSET_TYPE_MATERIAL_INSTANCE:
				return g_IconList.MaterialIcon->GetSRV();
			case HASH_ASSET_TYPE_LEVEL:
				return g_IconList.LevelIcon->GetSRV();
			case HASH_ASSET_TYPE_WIDGET:
				return g_IconList.WidgetIcon->GetSRV();
			case HASH_ASSET_TYPE_ANIMATION_CLIP:
				return g_IconList.AnimationIcon->GetSRV();
			case HASH_ASSET_TYPE_ANIMATOR:
				return g_IconList.AnimatorIcon->GetSRV();
			default:
				return g_IconList.FileIcon->GetSRV();
			}
		}();

		drawList->AddImage(iconTexture,
						   boxMin + ImVec2(2, 2),
						   boxMax - ImVec2(2, 2));


		if (bDisplayLabel)
		{
			ImU32 label_col = ImGui::GetColorU32(bIsItemSelected ? ImGuiCol_Text : ImGuiCol_TextDisabled);

			if (bRequestRename && bIsItemSelected)
			{
				static JText renameBuffer;

				// 버퍼 초기화 (처음 rename 모드로 들어올 때 한 번만 초기화)
				if (renameBuffer.empty())
				{
					renameBuffer = WString2String(itemData->FileName);
				}

				// InputText로 텍스트 편집기 제공
				// ImGui::SetKeyboardFocusHere();  // 자동으로 포커스를 InputText로 설정
				ImGui::InputText("##RenameText", &renameBuffer);
				if ((!ImGui::IsItemActive() && ImGui::IsMouseClicked(0)) || ImGui::IsKeyPressed(ImGuiKey_Enter))
				{
					bRequestRename = false;
					renameBuffer.append(".jasset");
					std::filesystem::rename(itemData->FilePath, itemData->FilePath.parent_path() / renameBuffer);
					renameBuffer.clear();
					OnBrowserChange.Execute();
				}

				// 편집을 취소하려면 esc 키 사용
				if (ImGui::IsKeyPressed(ImGuiKey_Escape))
				{
					bRequestRename = false;  // 편집 모드 종료
					renameBuffer.clear();     // 버퍼 초기화
				}
			}
			else
			{
				ImVec2 textPos = ImVec2(boxMin.x, boxMax.y - ImGui::GetFontSize());
				drawList->AddText(textPos, label_col, WString2String(itemData->FileName).c_str());
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
		const fs::path parentPath = fs::relative(mCurrentDirectory).parent_path();
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
		else if (!path.extension().empty())
		{
			if (StringHash(path.extension().c_str()) == Hash_EXT_JASSET)
			{
				mFiles.emplace_back(StringHash(path.c_str()), path.stem(), path, EFileType::Asset);
			}
			else if (path.extension() == ".png" || path.extension() == ".PNG")
			{
				mFiles.emplace_back(StringHash(path.c_str()), path.stem(), path, EFileType::Texture);
			}
		}
	}
}

void GUI_AssetBrowser::HandleAssetClicked(FBasicFilePreview* ItemData)
{
	JText fullFileName = ItemData->FilePath.string();

	const JAssetMetaData metaData = Utils::Serialization::GetType(fullFileName.c_str());

	uint32_t assetType = metaData.AssetType;

	switch (assetType)
	{
	case HASH_ASSET_TYPE_Actor:
		if (const auto newWindow = MGUIManager::Get().Load<GUI_Editor_Actor>(fullFileName, NAME_OBJECT_ACTOR))
		{
			newWindow->OpenIfNotOpened();
		}
		break;
	case HASH_ASSET_TYPE_Character:
		if (const auto newWindow = MGUIManager::Get().Load<GUI_Editor_Actor>(fullFileName, NAME_OBJECT_CHARACTER))
		{
			newWindow->OpenIfNotOpened();
		}
		break;
	case HASH_ASSET_TYPE_PLAYER_CHARACTER:
		if (const auto newWindow = MGUIManager::Get().Load<GUI_Editor_Actor>(fullFileName, NAME_OBJECT_PLAYER_CHARACTER))
		{
			newWindow->OpenIfNotOpened();
		}
		break;
	case HASH_ASSET_TYPE_Enemy:
		if (const auto newWindow = MGUIManager::Get().Load<GUI_Editor_Actor>(fullFileName, NAME_OBJECT_ENEMY))
		{
			newWindow->OpenIfNotOpened();
		}
		break;
	case HASH_ASSET_TYPE_KillerClown:
		if (const auto newWindow = MGUIManager::Get().Load<GUI_Editor_Actor>(fullFileName, NAME_OBJECT_KILLERCLOWN))
		{
			newWindow->OpenIfNotOpened();
		}
		break;	case HASH_ASSET_TYPE_INTERACTIVE_OBJECT:
		if (const auto newWindow = MGUIManager::Get().Load<GUI_Editor_Actor>(fullFileName, NAME_OBJECT_INTERACTIVE_OBJECT))
		{
			newWindow->OpenIfNotOpened();
		}
	case HASH_ASSET_TYPE_STATIC_MESH:
		if (const auto newWindow = MGUIManager::Get().Load<GUI_Editor_Mesh>(fullFileName))
		{
			newWindow->OpenIfNotOpened();
		}
		break;
	case HASH_ASSET_TYPE_SKELETAL_MESH:
		if (const auto newWindow = MGUIManager::Get().Load<GUI_Editor_SkeletalMesh>(fullFileName))
		{
			newWindow->OpenIfNotOpened();
		}
		break;
	case HASH_ASSET_TYPE_MATERIAL:
	case HASH_ASSET_TYPE_MATERIAL_INSTANCE:
		if (const auto newWindow = MGUIManager::Get().Load<GUI_Editor_Material>(fullFileName))
		{
			newWindow->OpenIfNotOpened();
		}
		break;

	case HASH_ASSET_TYPE_LEVEL:
		MLevelManager::Get().SetActiveLevel(MLevelManager::Get().Load(fullFileName));
		break;
	case HASH_ASSET_TYPE_WIDGET:
		if (const auto newWindow = MGUIManager::Get().Load<GUI_Editor_UI>(fullFileName))
		{
			newWindow->OpenIfNotOpened();
		}
		break;
	case HASH_ASSET_TYPE_ANIMATOR:
		if (const auto newWindow = MGUIManager::Get().Load<GUI_Editor_Animator>(fullFileName))
		{
			newWindow->OpenIfNotOpened();
		}
		break;
	default:
		LOG_CORE_WARN("Asset Browser : Unknown Asset Type");
		break;
	}
}

void GUI_AssetBrowser::OpenInputPopup(const JText& InAssetPath)
{
	static char inputBuffer[128] = ""; // 입력값 저장용 버퍼

	ImGui::OpenPopup("Actor");
	if (ImGui::BeginPopupModal("Actor", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text(u8("클래스 이름 입력(ex: AActor, ACharacter)"));
		ImGui::InputText("##WindowName", inputBuffer, IM_ARRAYSIZE(inputBuffer));

		if (ImGui::Button("Create"))
		{
			ImGui::CloseCurrentPopup();         // 팝업 닫기
			if (const auto newWindow = MGUIManager::Get().Load<GUI_Editor_Actor>(InAssetPath, inputBuffer))
			{
				newWindow->OpenIfNotOpened();
				OnBrowserChange.Execute();
				bOpenActorPopup = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();         // 팝업 닫기
			bOpenActorPopup = false;
		}

		ImGui::EndPopup();
	}
}
