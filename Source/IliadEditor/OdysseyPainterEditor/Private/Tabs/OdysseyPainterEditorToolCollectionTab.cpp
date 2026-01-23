// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorToolCollectionTab.h"
#include "Widgets/Input/SButton.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorCommands.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "Widgets/Views/STileView.h"
#include "Widgets/Tools/SOdysseyPainterEditorToolsTileView.h"
#include "Widgets/Tools/SOdysseyPainterEditorToolOptions.h"
#include "SOdysseyPainterEditorToolCollection.h"
#include "SOdysseyPainterEditorRasterDrawingToolBrushSelector.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyToolCollection.h"
#include "Editor/PropertyEditor/Public/PropertyCustomizationHelpers.h"
#include "SPositiveActionButton.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FOdysseyPainterEditorToolCollectionTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_ToolCollection");
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorToolCollectionTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorToolCollectionTab::~FOdysseyPainterEditorToolCollectionTab()
{
}

FOdysseyPainterEditorToolCollectionTab::FOdysseyPainterEditorToolCollectionTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT( "tool-collection-tab.name", "ToolCollection" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Tools" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyEditorTab interface

const FName&
FOdysseyPainterEditorToolCollectionTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorToolCollectionTab::CreateWidget()
{
    TSharedPtr<SWidget> widget = SNew(SVerticalBox)
        + SVerticalBox::Slot() //Recent tools
        .AutoHeight()
        [
            SNew(SOdysseyPainterEditorToolCollection)
                .Editor(mEditor)
                .ToolCollection(mEditor->GetRecentTools())
                .IsUnlocked(this, &FOdysseyPainterEditorToolCollectionTab::IsUnlocked)
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew( mTabOptions, SHorizontalBox )
                + SHorizontalBox::Slot()
                .FillWidth(1.f)
                .VAlign(VAlign_Center)
                [
                    SNew(SPositiveActionButton)
                    .Text(LOCTEXT("ToolCollection.add-tool-collection", "Add Tool Collection"))
                    .OnGetMenuContent(this, &FOdysseyPainterEditorToolCollectionTab::OnGetAddToolCollectionMenuContent)
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(SButton)
                        .ButtonStyle(FAppStyle::Get(), "NoBorder")
                        .ContentPadding(2)
                        .OnClicked(this, &FOdysseyPainterEditorToolCollectionTab::OnLockClicked)
                        [
                            SNew(SImage)
                                .Image(this, &FOdysseyPainterEditorToolCollectionTab::GetLockIcon)
                        ]
                ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(mCollectionsListView, SListView<TWeakObjectPtr<UOdysseyToolCollection>>)
                .ListItemsSource(&mSelectedCollections)
                .OnGenerateRow(this, &FOdysseyPainterEditorToolCollectionTab::OnGenerateCollectionRow)
                .SelectionMode(ESelectionMode::None)
        ];

    return widget;
}

void
FOdysseyPainterEditorToolCollectionTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorToolCollectionTab::__VA_ARGS__ ), FCanExecuteAction() );

    #undef MAP_ACTION
}

TSharedRef<ITableRow> FOdysseyPainterEditorToolCollectionTab::OnGenerateCollectionRow( TWeakObjectPtr<UOdysseyToolCollection> iCollection, const TSharedRef<STableViewBase>& iOwnerTable)
{
    return SNew(STableRow<TWeakObjectPtr<UOdysseyToolCollection>>, iOwnerTable)
        [
            SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(1.f)
                [
                    SNew(SOdysseyPainterEditorToolCollection)
                        .Editor(mEditor)
                        .ToolCollection(iCollection.Get())
                        .IsUnlocked( this, &FOdysseyPainterEditorToolCollectionTab::IsUnlocked )
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Top)
                [
                    SNew(SComboButton)
                    .HasDownArrow(true)
                    .OnGetMenuContent(this, &FOdysseyPainterEditorToolCollectionTab::GetCollectionMenuContent, iCollection)
                ]
        ];
}

TSharedRef<SWidget>
FOdysseyPainterEditorToolCollectionTab::OnGetAddToolCollectionMenuContent()
{
    TArray<FSoftObjectPath> loadedCollectionPaths;
    for (const TWeakObjectPtr<UOdysseyToolCollection>& collectionPtr : mSelectedCollections)
    {
        if (collectionPtr.IsValid())
        {
            loadedCollectionPaths.Add(FSoftObjectPath(collectionPtr.Get()));
        }
    }

    FOnShouldFilterAsset filterToolCollection = FOnShouldFilterAsset::CreateLambda(
        [loadedCollectionPaths](const FAssetData& AssetData)
        {
            return loadedCollectionPaths.Contains(AssetData.GetSoftObjectPath());
        });

    return PropertyCustomizationHelpers::MakeAssetPickerWithMenu(
        nullptr,
        false,
        false,
        { UOdysseyToolCollection::StaticClass() },
        {},
        PropertyCustomizationHelpers::GetNewAssetFactoriesForClasses({ UOdysseyToolCollection::StaticClass() }),
        filterToolCollection,
        FOnAssetSelected::CreateLambda(
            [this](const FAssetData& AssetData)
            {
                OnAssetSelected(AssetData);
            }
        ),
        FSimpleDelegate::CreateLambda(
            []()
            {
                FSlateApplication::Get().DismissAllMenus();
            }
        )
    );
}

FReply FOdysseyPainterEditorToolCollectionTab::OnLockClicked()
{
    bIsUnlocked = !bIsUnlocked;
    RefreshCollectionsGUI();
    return FReply::Handled();
}

bool FOdysseyPainterEditorToolCollectionTab::IsUnlocked() const
{
    return bIsUnlocked;
}

const FSlateBrush* FOdysseyPainterEditorToolCollectionTab::GetLockIcon() const
{
    return bIsUnlocked ? FAppStyle::Get().GetBrush("Icons.Unlock") : FAppStyle::Get().GetBrush("Icons.Lock");
}

void FOdysseyPainterEditorToolCollectionTab::RefreshCollectionsGUI()
{
    mSelectedCollections.Empty();

    if (mEditor)
    {
        for (UOdysseyToolCollection* collection : mEditor->GetToolCollections())
        {
            mSelectedCollections.Add(collection);
        }

        if (mCollectionsListView.IsValid())
        {
            mCollectionsListView->RequestListRefresh();
        }
    }
}

void FOdysseyPainterEditorToolCollectionTab::OnAssetSelected(const FAssetData& AssetData)
{
    UOdysseyToolCollection* collection = Cast<UOdysseyToolCollection>(AssetData.GetAsset());
    if (!collection)
        return;

    mEditor->AddToolCollection(collection);
    RefreshCollectionsGUI();
}

TSharedRef<SWidget> FOdysseyPainterEditorToolCollectionTab::GetCollectionMenuContent(TWeakObjectPtr<UOdysseyToolCollection> iCollection)
{
    FMenuBuilder menuBuilder(true, nullptr);

    menuBuilder.AddMenuEntry(
        FText::FromString(TEXT("Save Collection")),
        FText::FromString(TEXT("Save this tool collection")),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateSP(this, &FOdysseyPainterEditorToolCollectionTab::HandleSaveCollection, iCollection)
        )
    );

    menuBuilder.AddMenuEntry(
        FText::FromString(TEXT("Remove Collection")),
        FText::FromString(TEXT("Remove this tool collection from the editor")),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateSP(this, &FOdysseyPainterEditorToolCollectionTab::HandleRemoveCollection, iCollection)
        )
    );

    return menuBuilder.MakeWidget();
}

void FOdysseyPainterEditorToolCollectionTab::HandleSaveCollection(TWeakObjectPtr<UOdysseyToolCollection> iCollection)
{
    if (!iCollection.IsValid() || iCollection->IsCollectionTransient())
    {
        return;
    }

    UPackage* package = iCollection->GetOutermost();
    if (!package)
    {
        return;
    }

    if (!package->IsDirty())
    {
        return;
    }

    const FString packageFileName = FPackageName::LongPackageNameToFilename(package->GetName(), FPackageName::GetAssetPackageExtension());

    FSavePackageArgs saveArgs;
    saveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    saveArgs.SaveFlags = SAVE_None;
    saveArgs.Error = GError;

    UPackage::SavePackage(
        package,
        nullptr,
        *packageFileName,
        saveArgs
    );
}

void FOdysseyPainterEditorToolCollectionTab::HandleRemoveCollection(TWeakObjectPtr<UOdysseyToolCollection> iCollection)
{
    if (!iCollection.IsValid() || iCollection->IsCollectionTransient())
    {
        return;
    }

    mEditor->RemoveToolCollection(iCollection.Get());
    RefreshCollectionsGUI();
}

#undef LOCTEXT_NAMESPACE
