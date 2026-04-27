// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyPainterEditorToolTile.h"

#include "AssetToolsModule.h"
#include "Editor/ContentBrowser/Public/IContentBrowserSingleton.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "Engine/Texture2D.h"
#include "IAssetTools.h"
#include "OdysseyToolCollection.h"
#include "OdysseyToolCollectionDragDropOp.h"
#include "OdysseyPainterEditorRasterDrawingTool.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

static const FVector2D kTileSize = FVector2D(24.f, 24.f);

SOdysseyPainterEditorToolTile::~SOdysseyPainterEditorToolTile()
{
}

SOdysseyPainterEditorToolTile::SOdysseyPainterEditorToolTile()
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void
SOdysseyPainterEditorToolTile::Construct(const FArguments& InArgs)
{
    mToolConfig = InArgs._ToolConfig;
    mCollection = InArgs._ToolCollection;
    mEditor = InArgs._Editor;
    bIsUnlocked = InArgs._IsUnlocked;

    ChildSlot
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SNew(SOverlay)
                .IsEnabled( this, &SOdysseyPainterEditorToolTile::IsToolActivable )
                // Background layer (solid fill)
                + SOverlay::Slot()
                [
                    SNew(SColorBlock)
                        .Color(this, &SOdysseyPainterEditorToolTile::GetTileColor)
                ]
                + SOverlay::Slot() // foreground (icon)
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                .Padding(4)
                [
                    SNew(SImage)
                        .Image(this, &SOdysseyPainterEditorToolTile::GetIconBrush)
                        .ColorAndOpacity_Lambda([this]()
                            {
                                return mToolConfig ? mToolConfig->mIconToolConfiguration.mIconTint : FLinearColor::White;
                            })
                        .DesiredSizeOverride(kTileSize)
                ]
        ];
}

FReply SOdysseyPainterEditorToolTile::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bIsPressed = true;
        bIsDragged = false;

        return FReply::Handled()
            .DetectDrag(SharedThis(this), EKeys::LeftMouseButton)
            .CaptureMouse(SharedThis(this));
    }

    return FReply::Unhandled();
}

FReply SOdysseyPainterEditorToolTile::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
    if( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && HasMouseCapture() )
    {
        if( !bIsDragged )
        {
            if (mToolConfig && mEditor)
            {
                if (!IsToolActivable())
                    return FReply::Handled().ReleaseMouseCapture();

                UOdysseyPainterEditorTool* editorTool = mEditor->GetEditorToolOfClass(mToolConfig->mToolClass);
                if (editorTool && mToolConfig->mTool)
                {
                    UEngine::FCopyPropertiesForUnrelatedObjectsParams copyParams;
                    copyParams.bDoDelta = false;
                    UEngine::CopyPropertiesForUnrelatedObjects(mToolConfig->mTool, editorTool, copyParams);

                    // Particular case of UOdysseyPainterEditorRasterDrawingTool where we have to refresh the brush instance to the loaded tool
                    if (editorTool->IsA(UOdysseyPainterEditorRasterDrawingTool::StaticClass()))
                        Cast<UOdysseyPainterEditorRasterDrawingTool>(editorTool)->RefreshBrushInstance(false);

                    mEditor->ActivateMainTool(editorTool);
                }
            }
        }

        bIsPressed = false;
        bIsDragged = false;

        return FReply::Handled().ReleaseMouseCapture();
    }
    else if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        if (!IsUnlocked() && !mCollection->IsCollectionTransient())
            return FReply::Unhandled();

        FSlateApplication::Get().PushMenu(
            SharedThis(this),
            FWidgetPath(),
            BuildContextMenu(),
            MouseEvent.GetScreenSpacePosition(),
            FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu)
        );

        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FReply SOdysseyPainterEditorToolTile::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (!IsUnlocked())
        return FReply::Unhandled();

    if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        bIsDragged = true;
        return FReply::Handled().BeginDragDrop(FOdysseyToolCollectionDragDropOp::Create(mToolConfig, mCollection, mEditor));
    }
    return FReply::Unhandled();
}

FReply SOdysseyPainterEditorToolTile::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    if (!IsUnlocked())
        return FReply::Unhandled();

    auto dragOp = DragDropEvent.GetOperationAs<FOdysseyToolCollectionDragDropOp>();
    if (!dragOp.IsValid() || !mCollection || mCollection->IsCollectionTransient()) // No drop allowed in transient collection
        return FReply::Unhandled();

    UOdysseyPainterEditorToolConfiguration* sourceToolConfig = dragOp->GetToolConfig();
    UOdysseyToolCollection* sourceCollection = dragOp->GetSourceCollection().Get();
    FOdysseyPainterEditor* sourceEditor = dragOp->GetEditor();

    if (sourceCollection && sourceCollection != mCollection)
    {
        if(!sourceCollection->IsCollectionTransient())
            sourceCollection->RemoveToolConfiguration(sourceToolConfig);

        int32 targetIndex = mCollection->GetIndexOfToolConfiguration(mToolConfig);

        if (mDropSide == EDropIndicatorSide::Right)
            targetIndex++;

        UOdysseyPainterEditorToolConfiguration* toolConfig = mCollection->AddToolConfiguration( sourceToolConfig->mToolClass, sourceToolConfig->mTool, sourceToolConfig->mIconToolConfiguration, targetIndex );
    }
    else
    {
        int32 fromIndex = mCollection->GetIndexOfToolConfiguration(sourceToolConfig);
        int32 targetIndex = mCollection->GetIndexOfToolConfiguration(mToolConfig);

        if ( mDropSide == EDropIndicatorSide::Right )
            targetIndex++;

        mCollection->MoveToolConfiguration(fromIndex, targetIndex);
    }

    mDropSide = EDropIndicatorSide::None;

    return FReply::Handled();
}

FReply SOdysseyPainterEditorToolTile::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    if (!IsUnlocked())
        return FReply::Unhandled();

    auto dragOp = DragDropEvent.GetOperationAs<FOdysseyToolCollectionDragDropOp>();
    if (!dragOp.IsValid() || mCollection->IsCollectionTransient() )
        return FReply::Unhandled();

    FVector2D localPos = MyGeometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition());
    float halfWidth = MyGeometry.GetLocalSize().X * 0.5f;

    mDropSide = (localPos.X < halfWidth) ? EDropIndicatorSide::Left : EDropIndicatorSide::Right;

    return FReply::Handled();
}

void SOdysseyPainterEditorToolTile::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
    mDropSide = EDropIndicatorSide::None;
}

int32 SOdysseyPainterEditorToolTile::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
        int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    int32 RetLayer = SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    if (mDropSide != EDropIndicatorSide::None)
    {
        FVector2D size = AllottedGeometry.GetLocalSize();
        FLinearColor lineColor = FLinearColor::Green;
        float Thickness = 2.0f;

        FVector2D start, end;
        if (mDropSide == EDropIndicatorSide::Left)
        {
            start = FVector2D(0, 0);
            end   = FVector2D(0, size.Y);
        }
        else
        {
            start = FVector2D(size.X, 0);
            end   = FVector2D(size.X, size.Y);
        }

        FSlateDrawElement::MakeLines(
            OutDrawElements,
            RetLayer + 1,
            AllottedGeometry.ToPaintGeometry(),
            { start, end },
            ESlateDrawEffect::None,
            lineColor,
            true,
            Thickness
        );
    }
    return RetLayer + 1;
}

FLinearColor SOdysseyPainterEditorToolTile::GetTileColor() const
{
    if (!IsToolActivable())
        return FLinearColor::Transparent;

    if (bIsPressed && !bIsDragged)
        return FLinearColor(0.05f, 0.3f, 0.7f, 0.7f);
    else if(IsHovered())
        return FLinearColor(0.3f, 0.3f, 0.3f, 0.8f);
    else
        return FLinearColor::Transparent;
}

const FSlateBrush* SOdysseyPainterEditorToolTile::GetIconBrush() const
{
    if( mToolConfig )
        return mToolConfig->mIconToolConfiguration.MakeIconBrush();
    else
        return FStyleDefaults::GetNoBrush();
}

TSharedRef<SWidget> SOdysseyPainterEditorToolTile::BuildContextMenu()
{
    FMenuBuilder menuBuilder(true, nullptr);

    menuBuilder.AddMenuEntry(
        LOCTEXT( "tool-collection.tool-tile.context-menu.replace-with-current-tool.name", "Replace with Current Tool" ),
        LOCTEXT( "tool-collection.tool-tile.context-menu.replace-with-current-tool.tooltip", "Replaces this Tool with the Current Tool." ),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateSP(this, &SOdysseyPainterEditorToolTile::OnReplaceWithCurrentTool),
            FCanExecuteAction::CreateSP(this, &SOdysseyPainterEditorToolTile::CanReplaceWithCurrentTool)
        )
    );

    menuBuilder.AddMenuEntry(
        LOCTEXT( "tool-collection.tool-tile.context-menu.duplicate-tool.name", "Duplicate Tool" ),
        LOCTEXT( "tool-collection.tool-tile.context-menu.duplicate-tool.tooltip", "Create a duplicate of this tool." ),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateSP(this, &SOdysseyPainterEditorToolTile::OnDuplicateTool),
            FCanExecuteAction::CreateSP(this, &SOdysseyPainterEditorToolTile::CanDuplicateTool)
        )
    );

    menuBuilder.AddMenuEntry(
        LOCTEXT( "tool-collection.tool-tile.context-menu.delete-tool.name", "Delete Tool" ),
        LOCTEXT( "tool-collection.tool-tile.context-menu.delete-tool.tooltip", "Remove this tool from the collection." ),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateSP(this, &SOdysseyPainterEditorToolTile::OnDeleteTool),
            FCanExecuteAction::CreateSP(this, &SOdysseyPainterEditorToolTile::CanDeleteTool)
        )
    );

    menuBuilder.AddMenuEntry(
        LOCTEXT( "tool-collection.tool-tile.context-menu.change-icon.name", "Change Icon" ),
        LOCTEXT( "tool-collection.tool-tile.context-menu.change-icon.tooltip", "Select a new icon for this tool" ),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateSP(this, &SOdysseyPainterEditorToolTile::OnChangeIcon),
            FCanExecuteAction::CreateSP(this, &SOdysseyPainterEditorToolTile::CanChangeIcon)
        )
    );

    return menuBuilder.MakeWidget();
}

bool SOdysseyPainterEditorToolTile::IsUnlocked() const
{
    return bIsUnlocked.Get();
}

bool SOdysseyPainterEditorToolTile::IsToolActivable() const
{
    if( !mEditor )
        return false;

    UOdysseyPainterEditorTool* tool = mEditor->GetEditorToolOfClass(mToolConfig->mToolClass);

    if( !tool )
        return false;

    return tool->IsActivable();
}

bool SOdysseyPainterEditorToolTile::CanDeleteTool() const
{
    return (mCollection && mToolConfig);
}

void SOdysseyPainterEditorToolTile::OnDeleteTool()
{
    mCollection->RemoveToolConfiguration(mToolConfig);
}

bool
SOdysseyPainterEditorToolTile::CanReplaceWithCurrentTool() const
{
    if ( !mEditor || !mEditor->GetCurrentMainTool() || !mEditor->GetCurrentMainTool()->IsActivated() )
        return false;

    return (mCollection && mToolConfig && !mCollection->IsCollectionTransient());
}

void
SOdysseyPainterEditorToolTile::OnReplaceWithCurrentTool()
{
    if ( !mEditor || !mEditor->GetCurrentMainTool() || !mEditor->GetCurrentMainTool()->IsActivated() )
        return;

    FIconToolConfiguration iconToolConfig;
    iconToolConfig.mIconSource = EToolIconSource::Style;
    iconToolConfig.mIconStyleSet = mEditor->GetCurrentTool()->mIconStyleSet;


    int32 index = mCollection->GetIndexOfToolConfiguration(mToolConfig);
    mCollection->RemoveToolConfiguration(mToolConfig);
    mCollection->AddToolConfiguration( mEditor->GetCurrentTool()->GetClass(), mEditor->GetCurrentTool(), iconToolConfig, index );
}

bool SOdysseyPainterEditorToolTile::CanDuplicateTool() const
{
    return (mCollection && mToolConfig && !mCollection->IsCollectionTransient());
}

void SOdysseyPainterEditorToolTile::OnDuplicateTool()
{
    mCollection->AddToolConfiguration( mToolConfig->mToolClass, mToolConfig->mTool, mToolConfig->mIconToolConfiguration );
}

bool SOdysseyPainterEditorToolTile::CanChangeIcon() const
{
    return true;
}

TSharedRef<SWidget> SOdysseyPainterEditorToolTile::BuildTexturePicker()
{
    FAssetPickerConfig assetPickerConfig;
    assetPickerConfig.Filter.ClassNames.Add(UTexture2D::StaticClass()->GetFName());
    assetPickerConfig.Filter.bRecursiveClasses = false;
    assetPickerConfig.SelectionMode = ESelectionMode::Single;

    assetPickerConfig.OnShouldFilterAsset =
        FOnShouldFilterAsset::CreateLambda(
            [](const FAssetData& assetData)
            {
                const FString packagePath = assetData.PackagePath.ToString();

                bool bIsProjectAsset = packagePath.Equals("/Game") || packagePath.StartsWith("/Game/");

                if (!bIsProjectAsset || !assetData.GetClass()->IsChildOf(UTexture2D::StaticClass()))
                {
                    return true; // Filter out
                }

                return false; // Keep it
            }
        );

    assetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateSP( this, &SOdysseyPainterEditorToolTile::OnTextureSelected );

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

    return contentBrowserModule.Get().CreateAssetPicker(assetPickerConfig);
}

void SOdysseyPainterEditorToolTile::OpenTintColorPicker()
{
    FColorPickerArgs pickerArgs;
    pickerArgs.bIsModal = true;
    pickerArgs.bUseAlpha = true;
    pickerArgs.InitialColorOverride = mIconTint;
    pickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateLambda(
        [this](FLinearColor newColor)
        {
            mIconTint = newColor;
        }
    );

    OpenColorPicker(pickerArgs);
}

TSharedRef<SWidget> SOdysseyPainterEditorToolTile::BuildIconPicker( TSharedRef<SWindow> iPickerWindow )
{
    TArray< FName > filteredBrushesStyleSet;
    const FString filter = TEXT("ToolCollection");

    for (FName brushStyleSet : FOdysseyStyle::GetTrackedStyleSets())
    {
        if (brushStyleSet.ToString().Contains(filter))
        {
            filteredBrushesStyleSet.Add(brushStyleSet);
        }
    }

    filteredBrushesStyleSet.Sort([](const FName& iA, const FName& iB)
    {
        return iA.LexicalLess(iB);
    });

    // Wrap box for icon tiles
    TSharedRef<SWrapBox> iconWrapBox =
        SNew(SWrapBox)
        .UseAllottedSize(true)
        .InnerSlotPadding(FVector2D(4.f, 4.f));

    for (FName brushStyleSet : filteredBrushesStyleSet)
    {
        iconWrapBox->AddSlot()
            [
                SNew(SButton)
                    .ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
                    .OnClicked_Lambda([this, brushStyleSet, iPickerWindow]()
                        {
                            OnStyleIconSelected(brushStyleSet);
                            mIconTint = FLinearColor::White;
                            iPickerWindow->RequestDestroyWindow();
                            return FReply::Handled();
                        })
                    [
                        SNew(SImage)
                            .Image(FOdysseyStyle::GetBrush(brushStyleSet))
                            .DesiredSizeOverride(kTileSize)
                            .ColorAndOpacity_Lambda([this]()
                                {
                                    return mIconTint;
                                })
                    ]
            ];
    }

    return
        SNew(SVerticalBox)
            +SVerticalBox::Slot()
            .AutoHeight()
            .Padding(4.f)
            [
                SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                            .Text(FText::FromString("Icon Tint"))
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(8.f, 0.f)
                    [
                        SNew(SColorBlock)
                            .Color_Lambda([this]() { return mIconTint; })
                            .Size(FVector2D(32.f, 16.f))
                            .OnMouseButtonDown_Lambda([this](const FGeometry&, const FPointerEvent&)
                                {
                                    OpenTintColorPicker();
                                    return FReply::Handled();
                                })
                    ]
            ]
            + SVerticalBox::Slot()
            [
                SNew(SScrollBox)
                + SScrollBox::Slot()
                [
                    iconWrapBox
                ]
            ];
}

void SOdysseyPainterEditorToolTile::OnChangeIcon()
{
    TSharedRef<SWindow> pickerWindow = SNew(SWindow)
        .Title(FText::FromString("Select Icon"))
        .ClientSize(FVector2D(900, 500))
        .SupportsMinimize(false)
        .SupportsMaximize(false);

    TSharedRef<SWidget> assetPicker = BuildTexturePicker();
    TSharedRef<SWidget> iconPicker = BuildIconPicker(pickerWindow);

    TSharedRef<SSegmentedControl<EIconPickerTab>> segmentedControl =
        SNew(SSegmentedControl<EIconPickerTab>)
        .Value_Lambda([this]()
            {
                return mActiveTab;
            })
        .OnValueChanged_Lambda([this](EIconPickerTab newTab)
            {
                mActiveTab = newTab;
                mWidgetSwitcher->SetActiveWidgetIndex( newTab == EIconPickerTab::Textures ? 1 : 0 );
            })
        + SSegmentedControl<EIconPickerTab>::Slot(EIconPickerTab::Icons)
        .Text(FText::FromString("Icons"))
        + SSegmentedControl<EIconPickerTab>::Slot(EIconPickerTab::Textures)
        .Text(FText::FromString("Textures"));

    SAssignNew(mWidgetSwitcher, SWidgetSwitcher)
        + SWidgetSwitcher::Slot()
        [
            iconPicker
        ]
        + SWidgetSwitcher::Slot()
        [
            assetPicker
        ];

    mActiveTab = EIconPickerTab::Icons;
    mWidgetSwitcher->SetActiveWidgetIndex(0);

    pickerWindow->SetContent(
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(4)
        [
            segmentedControl
        ]

        + SVerticalBox::Slot()
        .FillHeight(1.f)
        [
            mWidgetSwitcher.ToSharedRef()
        ]
    );

    mPickerWindowPtr = pickerWindow;

    FSlateApplication::Get().AddModalWindow(
        pickerWindow,
        FSlateApplication::Get().GetActiveTopLevelWindow());
}


void SOdysseyPainterEditorToolTile::OnTextureSelected(const FAssetData& AssetData)
{
    if (!mToolConfig || !mCollection)
        return;

    if (UTexture2D* Texture = Cast<UTexture2D>(AssetData.GetAsset()))
    {
        mCollection->Modify();

        mToolConfig->mIconToolConfiguration.mIconSource = EToolIconSource::Texture;
        mToolConfig->mIconToolConfiguration.mIconTexture = Texture;
        mToolConfig->mIconToolConfiguration.mIconStyleSet = NAME_None;
        mToolConfig->mIconToolConfiguration.mIconTint = FLinearColor::White;

        mCollection->MarkPackageDirty();
    }

    if (mPickerWindowPtr.IsValid())
    {
        mPickerWindowPtr.Pin()->RequestDestroyWindow();
    }
}

void SOdysseyPainterEditorToolTile::OnStyleIconSelected(FName iBrushStyleSet)
{
    if (!mToolConfig || !mCollection)
        return;

    mCollection->Modify();

    mToolConfig->mIconToolConfiguration.mIconSource = EToolIconSource::Style;
    mToolConfig->mIconToolConfiguration.mIconStyleSet = iBrushStyleSet;
    mToolConfig->mIconToolConfiguration.mIconTexture = nullptr;
    mToolConfig->mIconToolConfiguration.mIconTint = mIconTint;

    mCollection->MarkPackageDirty();
}

#undef LOCTEXT_NAMESPACE
