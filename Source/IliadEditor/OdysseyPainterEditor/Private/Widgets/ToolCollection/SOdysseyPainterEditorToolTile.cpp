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
    mTool = InArgs._Tool;
    mCollection = InArgs._ToolCollection;
    mEditor = InArgs._Editor;

    ChildSlot
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SNew(SOverlay)

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
                        .Image(&mTool->Icon)
                        .DesiredSizeOverride(FVector2D(32.f, 32.f))
                ]
        ];
}

FReply SOdysseyPainterEditorToolTile::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        if (mTool && mEditor)
        {
            mTool->SetEditor(mEditor);
            mEditor->ActivateMainTool(mTool);
        }
        return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
    }
    else if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
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
    if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        return FReply::Handled().BeginDragDrop(FOdysseyToolCollectionDragDropOp::Create(mTool, mCollection, mEditor));
    }
    return FReply::Unhandled();
}

FReply SOdysseyPainterEditorToolTile::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    auto dragOp = DragDropEvent.GetOperationAs<FOdysseyToolCollectionDragDropOp>();
    if (!dragOp.IsValid() || !mCollection || mCollection->IsCollectionTransient()) // No drop allowed in transient collection

        return FReply::Unhandled();

    UOdysseyPainterEditorTool* sourceTool = dragOp->GetTool();
    UOdysseyToolCollection* sourceCollection = dragOp->GetSourceCollection().Get();
    FOdysseyPainterEditor* sourceEditor = dragOp->GetEditor();

    if (sourceCollection && sourceCollection != mCollection)
    {
        if(!sourceCollection->IsCollectionTransient())
            sourceCollection->RemoveTool(sourceTool);

        int32 targetIndex = mCollection->GetIndexOfTool(mTool);
        UOdysseyPainterEditorTool* tool = mCollection->AddTool(sourceTool, targetIndex);

        if (tool && mEditor)
        {
            tool->SetEditor(mEditor);
            mEditor->ActivateMainTool(tool);
        }

        if (sourceEditor != mEditor)
        {
            mEditor->InactivateMainTool();
        }
    }
    else
    {
        int32 fromIndex = mCollection->GetIndexOfTool(sourceTool);
        int32 targetIndex = mCollection->GetIndexOfTool(mTool);

        if ( mDropSide == EDropIndicatorSide::Right )
            targetIndex++;

        mCollection->MoveTool(fromIndex, targetIndex);
    }

    mDropSide = EDropIndicatorSide::None;

    return FReply::Handled();
}

FReply SOdysseyPainterEditorToolTile::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    auto dragOp = DragDropEvent.GetOperationAs<FOdysseyToolCollectionDragDropOp>();
    if (!dragOp.IsValid() || mCollection->IsCollectionTransient() )
        return FReply::Unhandled();

    FVector2D LocalPos = MyGeometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition());
    float HalfWidth = MyGeometry.GetLocalSize().X * 0.5f;

    mDropSide = (LocalPos.X < HalfWidth) ? EDropIndicatorSide::Left : EDropIndicatorSide::Right;

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
    if (!mEditor || !mTool)
        return FLinearColor::White; //Error

    if(IsHovered())
    {
        if(mEditor->GetCurrentTool() == mTool) //Hovered and tool is selected
            return FLinearColor(0.05f, 0.4f, 0.9f, 0.9f);
        else //Hovered not selected
            return FLinearColor(0.3f, 0.3f, 0.3f, 0.8f);
    }
    else
    {
        if (mEditor->GetCurrentTool() == mTool) //Non hovered and tool is selected
            return FLinearColor(0.05f, 0.3f, 0.7f, 0.7f);
        else //Non hovered and tool is not selected
            return FLinearColor::Transparent;
    }
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

bool SOdysseyPainterEditorToolTile::CanDeleteTool() const
{
    return (mCollection && mTool);
}

void SOdysseyPainterEditorToolTile::OnDeleteTool()
{
    mCollection->RemoveTool(mTool);
    //mEditor->ActivateMainTool(mEditor->FindDefaultToolForCurrentLayer());
    mEditor->InactivateMainTool();
}

bool SOdysseyPainterEditorToolTile::CanDuplicateTool() const
{
    return (mCollection && mTool && !mCollection->IsCollectionTransient());
}

void SOdysseyPainterEditorToolTile::OnDuplicateTool()
{
    UOdysseyPainterEditorTool* tool = mCollection->AddTool(mTool);
    tool->SetEditor(mEditor);
    mEditor->ActivateMainTool(tool);
}

bool SOdysseyPainterEditorToolTile::CanChangeIcon() const
{
    return true;
}

void SOdysseyPainterEditorToolTile::OnChangeIcon()
{
    TSharedRef<SWindow> PickerWindow = SNew(SWindow)
        .Title(FText::FromString("Select Icon"))
        .ClientSize(FVector2D(900, 500))
        .SupportsMinimize(false)
        .SupportsMaximize(false);

    // Texture Picker
    FAssetPickerConfig AssetPickerConfig;
    AssetPickerConfig.Filter.ClassNames.Add(UTexture2D::StaticClass()->GetFName());
    AssetPickerConfig.Filter.bRecursiveClasses = false;
    AssetPickerConfig.SelectionMode = ESelectionMode::Single;

    AssetPickerConfig.OnShouldFilterAsset = FOnShouldFilterAsset::CreateLambda(
        [](const FAssetData& AssetData)
        {
            return !AssetData.GetClass()->IsChildOf(UTexture2D::StaticClass());
        }
    );

    AssetPickerConfig.OnAssetSelected =
        FOnAssetSelected::CreateSP(this, &SOdysseyPainterEditorToolTile::OnTextureSelected);

    FContentBrowserModule& ContentBrowserModule =
        FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

    TSharedRef<SWidget> AssetPicker =
        ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig);

    // Icon Picker
    TArray<FName> ValidIcons;
    TSet<FName> StyleKeys = FAppStyle::Get().GetStyleKeys();

    for (const FName& Key : StyleKeys)
    {
        const FSlateBrush* Brush = FAppStyle::Get().GetBrush(Key);
        if (!Brush)
            continue;

        // Must have a resource
        if (Brush->GetResourceName().IsNone())
            continue;

        // Different tests to ditch checkerboard icons and small ones
        const FSlateResourceHandle Handle = FSlateApplication::Get().GetRenderer()->GetResourceHandle(*Brush);
        if (!Handle.IsValid())
        {
            continue;
        }

        const FSlateShaderResourceProxy* Proxy = Handle.GetResourceProxy();

        if (!Proxy || !Proxy->Resource)
            continue;

        if( Proxy->ActualSize.X < 32 || Proxy->ActualSize.Y < 32 )
            continue;

        ValidIcons.Add(Key);
    }

    // A WrapBox that wraps tiles automatically
    TSharedRef<SWrapBox> IconWrapBox =
        SNew(SWrapBox)
        .UseAllottedSize(true)          // resize to available width
        .InnerSlotPadding(FVector2D(4, 4));

    for (const FName& IconName : ValidIcons)
    {
        const FSlateBrush* Brush = FAppStyle::Get().GetBrush(IconName);

        IconWrapBox->AddSlot()
            .Padding(0)
            .HAlign(HAlign_Fill)
            [
                SNew(SButton)
                    .ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
                    .OnClicked_Lambda([this, IconName, PickerWindow]()
                        {
                            OnStyleIconSelected(IconName);
                            PickerWindow->RequestDestroyWindow();
                            return FReply::Handled();
                        })
                    [
                        SNew(SImage)
                            .Image(Brush)
                            .DesiredSizeOverride(FVector2D(32, 32)) // fixed icon size
                    ]
            ];
    }

    TSharedRef<SSplitter> Content =
        SNew(SSplitter)
        + SSplitter::Slot().Value(0.6f)
        [
            AssetPicker
        ]
        + SSplitter::Slot().Value(0.4f)
        [
            SNew(SScrollBox)
                + SScrollBox::Slot()
                [
                    IconWrapBox
                ]
        ];

    PickerWindow->SetContent(Content);
    FSlateApplication::Get().AddWindow(PickerWindow);
    PickerWindowPtr = PickerWindow;
}


void SOdysseyPainterEditorToolTile::OnTextureSelected(const FAssetData& AssetData)
{
    if( !mTool )
        return;

    UTexture2D* SelectedTexture = Cast<UTexture2D>(AssetData.GetAsset());
    if (SelectedTexture)
    {
        mTool->Icon.SetResourceObject(SelectedTexture);
        mTool->Icon.ImageSize = FVector2D(32, 32);
        // Close modal
        if (PickerWindowPtr.IsValid())
        {
            PickerWindowPtr.Pin()->RequestDestroyWindow();
        }
    }
}

void SOdysseyPainterEditorToolTile::OnStyleIconSelected(FName StyleIconName)
{
    if (!mTool)
        return;

    mTool->Icon = *FAppStyle::Get().GetBrush(StyleIconName);
}

/*
void
SOdysseyPainterEditorToolTile::OnToolCheckStateChanged(ECheckBoxState InValue, UOdysseyPainterEditorTool* iTool)
{

    if (InValue == ECheckBoxState::Checked)
        mOnToolSelected.ExecuteIfBound(iTool);
}

EVisibility
SOdysseyPainterEditorToolTile::ToolVisibility(UOdysseyPainterEditorTool* iTool) const
{
    return !iTool->mIsTemporaryTool && iTool->IsActivable() ? EVisibility::Visible : EVisibility::Collapsed;
}

ECheckBoxState
SOdysseyPainterEditorToolTile::IsToolChecked(UOdysseyPainterEditorTool* iTool) const
{
    return iTool->IsActivated() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

FText
SOdysseyPainterEditorToolTile::ToolTooltip(UOdysseyPainterEditorTool* iTool) const
{
    return iTool->GetTooltip();
}
*/
