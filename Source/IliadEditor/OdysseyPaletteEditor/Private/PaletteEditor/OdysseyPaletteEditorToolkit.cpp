// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "PaletteEditor/OdysseyPaletteEditorToolkit.h"

#include "AssetRegistry/IAssetRegistry.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Framework/Notifications/NotificationManager.h"
#include "ScopedTransaction.h"
#include "ToolMenu.h"
#include "ToolMenus.h"
#include "UObject/SavePackage.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyStyle.h"
#include "OdysseyTextureLayerImageVector.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseyLayerCell.h"
#include "OdysseyLayer.h"
#include "OdysseyMediaVector.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"
#include "Palette/OdysseyPalette.h"
#include "Palette/OdysseyPaletteEntryColor.h"
#include "Palette/OdysseyPaletteEntryFolder.h"
#include "PaletteEditor/OdysseyPaletteEditorToolMenuContext.h"
#include "OdysseyTelemetry.h"
#include "Widgets/SOdysseyPaletteTreeView.h"
#include "Widgets/SOdysseyPaletteSetComboBox.h"

#define LOCTEXT_NAMESPACE "PaletteEditor"

/////////////////////////////////////////////////////
// FOdysseyPaletteEditorToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPaletteEditorToolkit::~FOdysseyPaletteEditorToolkit()
{
    {
        using FAssetEditionFields = FAssetEdition_TelemetryFields;

        TArray<FAnalyticsEventAttribute> Attributes;
        Attributes.Emplace( FAssetEditionFields::EditorName_KeyName_AsString, GetToolkitFName() );
        Attributes.Emplace( FAssetEditionFields::SessionDuration_KeyName_AsDouble, ( FDateTime::UtcNow() - SessionStartTime ).GetTotalSeconds() );

        FOdysseyTelemetry::Get().RecordEvent( FAssetEditionFields::KeyName, Attributes );
    }
}

FOdysseyPaletteEditorToolkit::FOdysseyPaletteEditorToolkit(UOdysseyPalette* iPalette)
    : mColorsTabId("OdysseyPaletteEditor_Colors")
    , mPalette(iPalette)
{
    if( mPalette )
        mCurrentSet = mPalette->GetDefaultSetID();
    else
        mCurrentSet = FGuid();

    // Legacy, to remove next version, ensure that the Palettes are going to be saved with the upgraded data
    if (mPalette && mPalette->NeedsSavingAfterUpgrade)
    {
        mPalette->MarkPackageDirty();
        mPalette->NeedsSavingAfterUpgrade = false;
    }

    //---

    SessionStartTime = FDateTime::UtcNow();
}

void
FOdysseyPaletteEditorToolkit::Open()
{
    ExtendToolbar();

    TSharedRef<FTabManager::FLayout> layout = CreateLayout();
    FAssetEditorToolkit::InitAssetEditor( EToolkitMode::Standalone, NULL, "OdysseyPaletteEditor", layout, true, true, mPalette);
}

TSharedRef<FTabManager::FLayout>
FOdysseyPaletteEditorToolkit::CreateLayout()
{
    TSharedRef<FTabManager::FLayout> layout = FTabManager::NewLayout("OdysseyPaletteEditor");
    layout->AddArea(
        FTabManager::NewPrimaryArea()
        ->Split(
            FTabManager::NewStack()
            ->AddTab(mColorsTabId, ETabState::OpenedTab)
        )
    );

    return layout;
}

void
FOdysseyPaletteEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& iTabManager)
{
    FAssetEditorToolkit::RegisterTabSpawners(iTabManager);
    iTabManager->RegisterTabSpawner(mColorsTabId, FOnSpawnTab::CreateRaw(this, &FOdysseyPaletteEditorToolkit::SpawnColorsTab))
        .SetDisplayName(LOCTEXT("menu.tabs.colors.name", "Colors"))
        .SetIcon(FSlateIcon("OdysseyStyle", "PaletteEditor.Tab.Colors"))
        .SetGroup(AssetEditorTabsCategory.ToSharedRef());
}

void
FOdysseyPaletteEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& iTabManager)
{
    iTabManager->UnregisterTabSpawner(mColorsTabId);
}


TSharedRef<SDockTab>
FOdysseyPaletteEditorToolkit::SpawnColorsTab(const FSpawnTabArgs& Args)
{
    return SNew(SDockTab)
        .Label(LOCTEXT("tabs.colors.title", "Colors"))
        .CanEverClose(false) //We don't want the user to be able to close the list of colors
        [
            SAssignNew(mTreeView, SOdysseyPaletteTreeView)
            .IsReadOnly(false)
            .Palette(mPalette)
            .Set(this, &FOdysseyPaletteEditorToolkit::GetCurrentSet)
        ];
}

FText
FOdysseyPaletteEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT( "editor.name", "Odyssey Palette Editor" );
}

FName
FOdysseyPaletteEditorToolkit::GetToolkitFName() const
{
    return FName( "OdysseyPaletteEditor" );
}

FString
FOdysseyPaletteEditorToolkit::GetWorldCentricTabPrefix() const
{
    return LOCTEXT( "editor.world-centric-tab-prefix", "Palette " ).ToString();
}

FText
FOdysseyPaletteEditorToolkit::GetToolkitName() const
{
    return GetLabelForObject(GetEditingObjects()[0]);
}

FText
FOdysseyPaletteEditorToolkit::GetToolkitToolTipText() const
{
    return GetToolTipTextForObject(GetEditingObjects()[0]);
}
FLinearColor
FOdysseyPaletteEditorToolkit::GetWorldCentricTabColorScale() const
{
    return FLinearColor( 0.3f, 0.2f, 0.5f, 0.5f );
}

void
FOdysseyPaletteEditorToolkit::ExtendToolbar()
{
    const FName MenuName = GetToolMenuToolbarName();
    //Must be called before InitAssetEditor
    if (!UToolMenus::Get()->IsMenuRegistered(MenuName))
    {
        UToolMenu* ToolBar = UToolMenus::Get()->RegisterMenu(MenuName, "AssetEditor.DefaultToolBar", EMultiBoxType::ToolBar);

        ToolBar->AddDynamicSection(
            "PaletteSection",
            FNewToolBarDelegateLegacy::CreateLambda(
                [](FToolBarBuilder& iBuilder, UToolMenu* iToolMenu)
                {
                    const UOdysseyPaletteEditorToolMenuContext* Context = iToolMenu->FindContext<UOdysseyPaletteEditorToolMenuContext>();
                    if (!Context)
                        return;

                    FOdysseyPaletteEditorToolkit* toolkit = Context->Toolkit;
                    if (!toolkit)
                        return;

                    toolkit->BuildToolbarPaletteSection(iBuilder);
                }
            )
        );
    }
}

void
FOdysseyPaletteEditorToolkit::BuildToolbarPaletteSection(FToolBarBuilder& iBuilder)
{
    iBuilder.BeginSection("AddEntriesSection");

        iBuilder.AddToolBarButton(
            FUIAction(FExecuteAction::CreateRaw(this, &FOdysseyPaletteEditorToolkit::AddEntry, UOdysseyPaletteEntryColor::StaticClass())),
            "AddColor",
            LOCTEXT("toolbar.add-color.name", "Add Color"),
            LOCTEXT("toolbar.add-color.tooltip", "Adds a color to the palette"),
            FSlateIcon("OdysseyStyle", "OdysseyPalette.AddColor")
        );

        iBuilder.AddToolBarButton(
            FUIAction(FExecuteAction::CreateRaw(this, &FOdysseyPaletteEditorToolkit::AddEntry, UOdysseyPaletteEntryFolder::StaticClass())),
            "AddFolder",
            LOCTEXT("toolbar.add-folder.name", "Add Folder"),
            LOCTEXT("toolbar.add-folder.tooltip", "Adds a folder to the palette"),
            FSlateIcon("OdysseyStyle", "OdysseyPalette.AddFolder")
        );

    iBuilder.EndSection();

    iBuilder.BeginSection("SetSection");

        iBuilder.AddWidget(
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .Padding(FMargin(10, 0, 4, 0))
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("toolbar.set.name", "Set"))
            ]
            + SHorizontalBox::Slot()
            .Padding(FMargin(0, 0, 10, 0))
            .AutoWidth()
            [
                SNew(SOdysseyPaletteSetComboBox)
                .IsReadOnly(false)
                .Palette(mPalette)
                .CurrentSet(this, &FOdysseyPaletteEditorToolkit::GetCurrentSet)
                .OnCurrentSetSelected(this, &FOdysseyPaletteEditorToolkit::OnCurrentSetSelected)
            ],
            FMenuEntryStyleParams(),
            NAME_None,
            true,
            FNewMenuDelegate::CreateLambda(
                [this](FMenuBuilder& iMenuBuilder)
                {
                    SOdysseyPaletteSetComboBox::BuildMenu(
                        iMenuBuilder,
                        mPalette,
                        mCurrentSet,
                        false,
                        SOdysseyPaletteSetComboBox::FOnCurrentSetSelected::CreateRaw(this, &FOdysseyPaletteEditorToolkit::OnCurrentSetSelected)
                    );
                }
            )
        );

    iBuilder.EndSection();

    iBuilder.BeginSection("RefreshSection");

    iBuilder.AddWidget(
        SAssignNew(mRefreshButton, SButton)
        .ButtonStyle(FAppStyle::Get(), "SimpleButton")
        .OnClicked(this, &FOdysseyPaletteEditorToolkit::OnRefreshClicked)
        .ToolTipText(NSLOCTEXT("OdysseyPaletteEditorToolkit", "RefreshTooltip", "Refresh the assets that uses this palette"))
        .ContentPadding(FMargin(4, 2))
        .Content()
        [
            SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(SImage)
                        .Image(FAppStyle::Get().GetBrush("AssetEditor.Apply"))
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                .Padding(FMargin(4, 0, 0, 0))
                [
                    SNew(STextBlock)
                        .Text(NSLOCTEXT("OdysseyPaletteEditorToolkit", "Refresh", "Refresh"))
                        .TextStyle(FAppStyle::Get(), "NormalText")
                ]
        ]
    );

    iBuilder.EndSection();

}

void
FOdysseyPaletteEditorToolkit::AddEntry(UClass* iEntryClass)
{
#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("transaction.add-color-entry", "Add Color Entry"));
#endif

    UOdysseyPaletteEntry* currentEntry = nullptr;

    if (mTreeView) //mTreeView can be null if the treeview tab is not opened at toolkit startup
    {
        TArray< UOdysseyPaletteEntry* > selectedEntries = mTreeView->GetSelectedItems();
        if (!selectedEntries.IsEmpty())
        {
            currentEntry = selectedEntries.Last();
        }
    }

    if (currentEntry)
    {
        if (currentEntry->CanHaveChildren)
        {
            currentEntry = mPalette->AddEntry(iEntryClass, currentEntry);
        }
        else
        {
            UOdysseyPaletteEntry* parent = currentEntry->GetParent();
            int index = currentEntry->GetIndexInParent();
            currentEntry = mPalette->AddEntry(iEntryClass, parent, index);
        }
    }
    else
    {
        currentEntry = mPalette->AddEntry(iEntryClass);
    }
}

void
FOdysseyPaletteEditorToolkit::InitToolMenuContext(FToolMenuContext& MenuContext)
{
    UOdysseyPaletteEditorToolMenuContext* Context = NewObject<UOdysseyPaletteEditorToolMenuContext>();
    Context->Toolkit = this;
    MenuContext.AddObject(Context);
}

FGuid
FOdysseyPaletteEditorToolkit::GetCurrentSet() const
{
    return mCurrentSet;
}

void
FOdysseyPaletteEditorToolkit::OnCurrentSetSelected(FGuid iSet)
{
    mCurrentSet = iSet;
}

FReply
FOdysseyPaletteEditorToolkit::OnRefreshClicked()
{
    mPalette->RefreshReferencedAssets();
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
