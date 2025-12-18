// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/ToolCollection/SOdysseyPainterEditorToolTile.h"

#include "AssetToolsModule.h"
#include "Editor/ContentBrowser/Public/IContentBrowserSingleton.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "Engine/Texture2D.h"
#include "IAssetTools.h"
#include "ToolCollection/OdysseyToolCollection.h"
#include "ToolCollection/OdysseyToolCollectionDragDropOp.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Colors/SColorBlock.h"



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

                // Glow overlay
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
                        .Image(&mToolConfig->mIcon)
                        .DesiredSizeOverride(FVector2D(32.f, 32.f))
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
                if (editorTool)
                {
                    mEditor->LoadToolFromPropertySnapshot(editorTool, mToolConfig->mSnapshot);

                    // Particular case of UOdysseyPainterEditorRasterDrawingTool where we have to refresh the brush instance to the loaded tool
                    if (editorTool->IsA(UOdysseyPainterEditorRasterDrawingTool::StaticClass()))
                        Cast<UOdysseyPainterEditorRasterDrawingTool>(editorTool)->RefreshBrushInstance();

                    /*else if (editorTool->IsA(UOdysseyPainterEditorAnimationOutOfPegsTool::StaticClass()))
                        Cast<UOdysseyPainterEditorAnimationOutOfPegsTool>(editorTool)->Load();*/

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
        if (!IsUnlocked())
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

        UOdysseyPainterEditorToolConfiguration* toolConfig = mCollection->AddToolConfiguration( sourceToolConfig->mToolClass, sourceToolConfig->mSnapshot, sourceToolConfig->mIcon, targetIndex );
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

TSharedRef<SWidget> SOdysseyPainterEditorToolTile::BuildContextMenu()
{
    FMenuBuilder menuBuilder(true, nullptr);

    menuBuilder.AddMenuEntry(
        FText::FromString(TEXT("Duplicate Tool")),
        FText::FromString(TEXT("Create a duplicate of this tool.")),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateSP(this, &SOdysseyPainterEditorToolTile::OnDuplicateTool),
            FCanExecuteAction::CreateSP(this, &SOdysseyPainterEditorToolTile::CanDuplicateTool)
        )
    );

    menuBuilder.AddMenuEntry(
        FText::FromString(TEXT("Delete Tool")),
        FText::FromString(TEXT("Remove this tool from the collection.")),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateSP(this, &SOdysseyPainterEditorToolTile::OnDeleteTool),
            FCanExecuteAction::CreateSP(this, &SOdysseyPainterEditorToolTile::CanDeleteTool)
        )
    );

    menuBuilder.AddMenuEntry(
        FText::FromString(TEXT("Change Icon")),
        FText::FromString(TEXT("Select a new icon for this tool")),
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

bool SOdysseyPainterEditorToolTile::CanDuplicateTool() const
{
    return (mCollection && mToolConfig && !mCollection->IsCollectionTransient());
}

void SOdysseyPainterEditorToolTile::OnDuplicateTool()
{
    mCollection->AddToolConfiguration(mToolConfig->mToolClass, mToolConfig->mSnapshot, mToolConfig->mIcon );
}

bool SOdysseyPainterEditorToolTile::CanChangeIcon() const
{
    return true;
}

void SOdysseyPainterEditorToolTile::OnChangeIcon()
{
    TSharedRef<SWindow> pickerWindow = SNew(SWindow)
        .Title(FText::FromString("Select Icon"))
        .ClientSize(FVector2D(900, 500))
        .SupportsMinimize(false)
        .SupportsMaximize(false);

    // Texture Picker
    FAssetPickerConfig assetPickerConfig;
    assetPickerConfig.Filter.ClassNames.Add(UTexture2D::StaticClass()->GetFName());
    assetPickerConfig.Filter.bRecursiveClasses = false;
    assetPickerConfig.SelectionMode = ESelectionMode::Single;

    assetPickerConfig.OnShouldFilterAsset = FOnShouldFilterAsset::CreateLambda(
        [](const FAssetData& AssetData)
        {
            return !AssetData.GetClass()->IsChildOf(UTexture2D::StaticClass());
        }
    );

    assetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateSP(this, &SOdysseyPainterEditorToolTile::OnTextureSelected);

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

    TSharedRef<SWidget> assetPicker = contentBrowserModule.Get().CreateAssetPicker(assetPickerConfig);

    // Icon Picker
    TArray<FName> validIcons;
    TSet<FName> styleKeys = FAppStyle::Get().GetStyleKeys();

    for (const FName& key : styleKeys)
    {
        const FSlateBrush* brush = FAppStyle::Get().GetBrush(key);
        if (!brush)
            continue;

        // Must have a resource
        if (brush->GetResourceName().IsNone())
            continue;

        // Different tests to ditch checkerboard icons and small ones
        const FSlateResourceHandle handle = FSlateApplication::Get().GetRenderer()->GetResourceHandle(*brush);
        if (!handle.IsValid())
        {
            continue;
        }

        const FSlateShaderResourceProxy* proxy = handle.GetResourceProxy();

        if (!proxy || !proxy->Resource)
            continue;

        if( proxy->ActualSize.X < 32 || proxy->ActualSize.Y < 32 )
            continue;

        validIcons.Add(key);
    }

    // A WrapBox that wraps tiles automatically
    TSharedRef<SWrapBox> iconWrapBox =
        SNew(SWrapBox)
        .UseAllottedSize(true)          // resize to available width
        .InnerSlotPadding(FVector2D(4, 4));

    for (const FName& iconName : validIcons)
    {
        const FSlateBrush* brush = FAppStyle::Get().GetBrush(iconName);

        iconWrapBox->AddSlot()
            .Padding(0)
            .HAlign(HAlign_Fill)
            [
                SNew(SButton)
                    .ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
                    .OnClicked_Lambda([this, iconName, pickerWindow]()
                        {
                            OnStyleIconSelected(iconName);
                            pickerWindow->RequestDestroyWindow();
                            return FReply::Handled();
                        })
                    [
                        SNew(SImage)
                            .Image(brush)
                            .DesiredSizeOverride(FVector2D(32, 32)) // fixed icon size
                    ]
            ];
    }

    TSharedRef<SSplitter> content =
        SNew(SSplitter)
        + SSplitter::Slot().Value(0.6f)
        [
            assetPicker
        ]
        + SSplitter::Slot().Value(0.4f)
        [
            SNew(SScrollBox)
                + SScrollBox::Slot()
                [
                    iconWrapBox
                ]
        ];

    pickerWindow->SetContent(content);
    FSlateApplication::Get().AddWindow(pickerWindow);
    mPickerWindowPtr = pickerWindow;
}


void SOdysseyPainterEditorToolTile::OnTextureSelected(const FAssetData& AssetData)
{
    if( !mToolConfig || !mCollection )
        return;

    UTexture2D* SelectedTexture = Cast<UTexture2D>(AssetData.GetAsset());
    if( SelectedTexture )
    {
        FSlateBrush newIcon;
        newIcon.SetResourceObject(SelectedTexture);
        newIcon.ImageSize = FVector2D(32, 32);
        mCollection->Modify();
        mToolConfig->mIcon = newIcon;
        mCollection->MarkPackageDirty();
        // Close modal
        if (mPickerWindowPtr.IsValid())
        {
            mPickerWindowPtr.Pin()->RequestDestroyWindow();
        }
    }
}

void SOdysseyPainterEditorToolTile::OnStyleIconSelected(FName StyleIconName)
{
    if (!mToolConfig || !mCollection)
        return;

    mCollection->Modify();
    mToolConfig->mIcon = *FAppStyle::Get().GetBrush(StyleIconName);
    mCollection->MarkPackageDirty();
}
