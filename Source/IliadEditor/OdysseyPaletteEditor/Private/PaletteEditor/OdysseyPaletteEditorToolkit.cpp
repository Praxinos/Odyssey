// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "PaletteEditor/OdysseyPaletteEditorToolkit.h"

#include "AssetRegistry/IAssetRegistry.h"
#include "AssetRegistry/AssetRegistryModule.h"

#include "Palette/OdysseyPalette.h"
#include "Palette/OdysseyPaletteEntryColor.h"
#include "Palette/OdysseyPaletteEntryFolder.h"

#include "PaletteEditor/OdysseyPaletteEditorToolMenuContext.h"

#include "UObject/SavePackage.h"

#include "Widgets/SOdysseyPaletteTreeView.h"
#include "Widgets/SOdysseyPaletteSetComboBox.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyStyle.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseyLayerCell.h"
#include "OdysseyLayer.h"
#include "OdysseyMediaVector.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"

#define LOCTEXT_NAMESPACE "PaletteEditor"

/////////////////////////////////////////////////////
// FOdysseyPaletteEditorToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPaletteEditorToolkit::~FOdysseyPaletteEditorToolkit()
{
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

    iBuilder.BeginSection("ApplySection");

    iBuilder.AddWidget(
        SAssignNew(mApplyButton, SButton)
        .ButtonStyle(FAppStyle::Get(), "SimpleButton")
        .OnClicked(this, &FOdysseyPaletteEditorToolkit::OnApplyClicked)
        .ToolTipText(NSLOCTEXT("OdysseyPaletteEditorToolkit", "ApplyTooltip", "Apply the palette and its colors to all assets that use it"))
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
                        .Text(NSLOCTEXT("OdysseyPaletteEditorToolkit", "Apply", "Apply"))
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
    TArray< UOdysseyPaletteEntry* > selectedEntries = mTreeView->GetSelectedItems();
    if (!selectedEntries.IsEmpty())
    {
        currentEntry = selectedEntries.Last();
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
FOdysseyPaletteEditorToolkit::OnApplyClicked()
{
    int numReferencedAssets = GetReferencedAssetsViaAssetRegistry().Num();
    if( numReferencedAssets == 0 )
        return FReply::Handled();

    FText confirmText = FText::Format(
        NSLOCTEXT("PaletteEditor", "ConfirmApply", "Are you sure you want to apply this palette to {0} assets?"),
        FText::AsNumber(numReferencedAssets)
    );

    FSlateApplication::Get().PushMenu(
        mApplyButton->AsShared(),
        FWidgetPath(),
        SNew(SBorder)
        .Padding(10)
        .BorderImage(FCoreStyle::Get().GetBrush("Menu.Background"))
        [
            SNew(SVerticalBox)

                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(5)
                [
                    SNew(STextBlock)
                        .Text(confirmText)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                .HAlign(HAlign_Right)
                .Padding(5)
                [
                    SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(2)
                        [
                            SNew(SButton)
                                .Text(FText::FromString("Confirm"))
                                .OnClicked_Lambda([this]() {
                                FSlateApplication::Get().DismissAllMenus();
                                ApplyPaletteToReferencedAssets(); //Apply palette
                                return FReply::Handled();
                                    })
                        ]

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(2)
                        [
                            SNew(SButton)
                                .Text(FText::FromString("Cancel"))
                                .OnClicked_Lambda([]() {
                                FSlateApplication::Get().DismissAllMenus();
                                return FReply::Handled();
                                })
                        ]

                ]
        ],
        FSlateApplication::Get().GetCursorPos(),
        FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu)
    );

    return FReply::Handled();
}

void FOdysseyPaletteEditorToolkit::ApplyPaletteToReferencedAssets()
{
    for (FName assetName : GetReferencedAssetsViaAssetRegistry())
    {
        FString pathStr = assetName.ToString();
        FString assetType = FPackageName::GetShortName(pathStr);
        FString fullObjectPath = FString::Printf(TEXT("%s.%s"), *pathStr, *assetType);

        TSoftObjectPtr<UOdysseyAnimation> animation(fullObjectPath);
        TSoftObjectPtr<UTexture2D> texture(fullObjectPath);

        if (animation.IsValid()) // The asset is valid AND loaded
        {
            //Save the animation before applying the palette, because all these refresh only affect the "on disk" version of the asset, not the dirty "on memory" one.
            //Therefore, if the palette is freshly added to the (then dirtied) animation, the refresh won't apply to it
            /*UPackage* package = animation->GetOutermost();
            FSavePackageArgs saveArgs;
            saveArgs.TopLevelFlags = RF_Standalone;
            saveArgs.Error = GWarn;
            saveArgs.SaveFlags = SAVE_NoError;
            FTimerHandle TimerHandle;
            UPackage::SavePackage(package, animation.Get(), *package->GetName(), saveArgs);
            FlushAsyncLoading();*/

            TArray<UOdysseyLayer*> layers = animation->GetLayerStack()->GetLayers();
            for (UOdysseyLayer* layer : layers)
            {
                for (UOdysseyLayerCell* cell : layer->GetCells())
                {
                    UOdysseyAnimationCellImageVector* animationVectorCell;
                    if (cell->IsA(UOdysseyAnimationCellImageVector::StaticClass()))
                        animationVectorCell = Cast<UOdysseyAnimationCellImageVector>(cell);
                    else
                        continue;

                    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = animationVectorCell->GetMediaProvider().GetMedias<FOdysseyMediaVector>();

                    if (mediaVectors.Num() > 0)
                    {
                        for (int i = 0; i < mediaVectors.Num(); i++)
                        {
                            FOdysseyVectorCell* vectorCell = mediaVectors[i]->GetScene()->GetCell();
                            vectorCell->GetLayer()->RequestRedraw(vectorCell, 0);
                        }
                    }
                }
            }
        }
        else if (texture.IsValid()) // The asset is valid AND loaded
        {
            UOdysseyTextureLayerStackUserData* textureUserData = Cast<UOdysseyTextureLayerStackUserData>(texture->GetAssetUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass()));
            if (!textureUserData)
                continue;

            /*UPackage* package = texture->GetOutermost();
            FSavePackageArgs saveArgs;
            saveArgs.TopLevelFlags = RF_Standalone;
            saveArgs.Error = GWarn;
            saveArgs.SaveFlags = SAVE_NoError;
            FTimerHandle TimerHandle;
            UPackage::SavePackage(package, texture.Get(), *package->GetName(), saveArgs);
            FlushAsyncLoading();
            texture->UpdateResource();
            texture->PostEditChange();*/

            TArray<UOdysseyLayer*> layers = textureUserData->GetLayerStack()->GetLayers();
            for (UOdysseyLayer* layer : layers)
            {
                if (!layer->IsA(UOdysseyTextureLayerImageVector::StaticClass()))
                    continue;

                UOdysseyTextureLayerImageVector* vectorLayer = Cast<UOdysseyTextureLayerImageVector>(layer);

                TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = vectorLayer->GetMediaProvider(0).GetMedias<FOdysseyMediaVector>();

                if (mediaVectors.Num() > 0)
                {
                    for (int i = 0; i < mediaVectors.Num(); i++)
                    {
                        FOdysseyVectorCell* vectorCell = mediaVectors[i]->GetScene()->GetCell();
                        vectorCell->GetLayer()->RequestRedraw(vectorCell, 0);
                    }
                }
            }
        }
    }

}

TArray<FName>
FOdysseyPaletteEditorToolkit::GetReferencedAssetsViaAssetRegistry()
{
    TArray<FName> outDependencies;

    if (!mPalette)
        return outDependencies;

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    FAssetData AssetData = AssetRegistry.GetAssetByObjectPath(FSoftObjectPath(mPalette->GetPathName()));
    FName AssetPath;

    if (AssetData.IsValid())
    {
        AssetPath = AssetData.PackageName;
    }

    AssetRegistry.GetReferencers(AssetPath, outDependencies);

    return outDependencies;
}

#undef LOCTEXT_NAMESPACE
