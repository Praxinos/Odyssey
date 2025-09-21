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
#include "Widgets/ToolCollection/SOdysseyPainterEditorToolCollection.h"
#include "Tools/RasterDrawingTool/Widgets/SOdysseyPainterEditorRasterDrawingToolBrushSelector.h"
#include "OdysseyPainterEditorCommands.h"
#include "ToolCollection/OdysseyToolCollection.h"
#include "Editor/PropertyEditor/Public/PropertyCustomizationHelpers.h"
#include "SPositiveActionButton.h"

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
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SPositiveActionButton)
                .Text(LOCTEXT("ToolCollection.add-tool-collection", "Add Tool Collection"))
                .OnGetMenuContent(this, &FOdysseyPainterEditorToolCollectionTab::OnGetAddToolCollectionMenuContent)
        ]
        + SVerticalBox::Slot() //Recent tools
        .AutoHeight()
        [
            SNew(SOdysseyPainterEditorToolCollection)
                .Editor(mEditor)
                .ToolCollection(mEditor->GetRecentTools())
        ]
        + SVerticalBox::Slot().FillHeight(1.f)
        [
            SAssignNew(mCollectionsWidget, SVerticalBox)
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

TSharedRef<SWidget>
FOdysseyPainterEditorToolCollectionTab::OnGetAddToolCollectionMenuContent()
{
    RefreshCollectionsUI();

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

void FOdysseyPainterEditorToolCollectionTab::RefreshCollectionsUI()
{
    mCollectionsWidget->ClearChildren();
    mSelectedCollections.Empty();

    if (mEditor)
    {
        for (UOdysseyToolCollection* collection : mEditor->GetToolCollections())
        {
            mSelectedCollections.Add(collection);
        }
    }

    for(TWeakObjectPtr<UOdysseyToolCollection> collection : mSelectedCollections)
    {
        mCollectionsWidget->AddSlot().AutoHeight()
        [
            SNew(SOdysseyPainterEditorToolCollection)
                .Editor(mEditor)
                .ToolCollection(collection.Get())
        ];
    }
}

void FOdysseyPainterEditorToolCollectionTab::OnAssetSelected(const FAssetData& AssetData)
{
    UOdysseyToolCollection* collection = Cast<UOdysseyToolCollection>(AssetData.GetAsset());
    if (!collection)
        return;

    mSelectedCollections.Add(collection);
    mEditor->AddToolCollection(collection);

    mCollectionsWidget->AddSlot().AutoHeight()
    [
        SNew(SOdysseyPainterEditorToolCollection)
            .Editor(mEditor)
            .ToolCollection(collection)
    ];
}

#undef LOCTEXT_NAMESPACE
