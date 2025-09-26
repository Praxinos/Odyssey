// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/ToolCollection/SOdysseyPainterEditorToolTile.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "ToolCollection/OdysseyToolCollection.h"
#include "ToolCollection/OdysseyToolCollectionDragDropOp.h"

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

    ChildSlot
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        .Padding(4)
        [
            SNew(SImage)
                .Image(&mTool->Icon)
                .DesiredSizeOverride(FVector2D(32.f, 32.f))
        ];
}

FReply SOdysseyPainterEditorToolTile::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
    }
    return FReply::Unhandled();
}

FReply SOdysseyPainterEditorToolTile::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        return FReply::Handled().BeginDragDrop(FOdysseyToolCollectionDragDropOp::Create(mTool, mCollection));
    }
    return FReply::Unhandled();
}

TOptional<EItemDropZone> SOdysseyPainterEditorToolTile::HandleCanAcceptDrop(
    const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, UOdysseyPainterEditorTool* TargetItem)
{
    auto DragOp = DragDropEvent.GetOperationAs<FOdysseyToolCollectionDragDropOp>();
    if (DragOp.IsValid())
    {
        UE_LOG(LogTemp, Display, TEXT("CAN ACCEPT DROP"));
        return DropZone; // Allow before/after/onto
    }
    return TOptional<EItemDropZone>();
}

FReply SOdysseyPainterEditorToolTile::HandleAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, UOdysseyPainterEditorTool* TargetItem)
{
    auto DragOp = DragDropEvent.GetOperationAs<FOdysseyToolCollectionDragDropOp>();
    if (!DragOp.IsValid() || !mCollection)
        return FReply::Unhandled();

    UOdysseyPainterEditorTool* Tool = DragOp->GetTool();
    UOdysseyToolCollection* Source = DragOp->GetSourceCollection().Get();

    if (Source && Source != mCollection)
    {
        Source->RemoveTool(Tool);
    }

    /*int32 TargetIndex = mCollection->FindToolIndex(TargetItem);
    if (TargetIndex == INDEX_NONE)
    {*/
        mCollection->AddTool(Tool);
    /*}
    else
    {
        if (DropZone == EItemDropZone::BelowItem)
        {
            TargetIndex++;
        }
        mCollection->InsertToolAt(Tool, TargetIndex);
    }*/

    UE_LOG(LogTemp, Display, TEXT("ACCEPT DROP"));

    return FReply::Handled();
}

FReply SOdysseyPainterEditorToolTile::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    auto DragOp = DragDropEvent.GetOperationAs<FOdysseyToolCollectionDragDropOp>();
    if (!DragOp.IsValid()) return FReply::Unhandled();

    FVector2D LocalPos = MyGeometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition());
    float HalfWidth = MyGeometry.GetLocalSize().X * 0.5f;

    mDropSide = (LocalPos.X < HalfWidth) ? EDropIndicatorSide::Left : EDropIndicatorSide::Right;

    return FReply::Handled();
}

void SOdysseyPainterEditorToolTile::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
    mDropSide = EDropIndicatorSide::None;
}

/*
FReply SOdysseyPainterEditorToolTile::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    auto DragOp = DragDropEvent.GetOperationAs<FOdysseyToolCollectionDragDropOp>();
    if (DragOp.IsValid() && mCollection && DragOp->GetTool())
    {
        UOdysseyPainterEditorTool* Tool = DragOp->GetTool();
        UOdysseyToolCollection* Source = DragOp->GetSourceCollection().Get();

        if (Source && Source != mCollection)
        {
            Source->RemoveTool(Tool);
        }

        //int32 TargetIndex = mCollection->FindToolIndex(mTool); // hovered tool
        int32 TargetIndex = 0;
        if (TargetIndex != INDEX_NONE)
        {
            if (mDropSide == EDropIndicatorSide::Right)
            {
                TargetIndex++; // insert after
            }
            //mCollection->InsertToolAt(Tool, TargetIndex);
            mCollection->AddTool(Tool);
        }
        else
        {
            // If for some reason target tool not found, just append
            mCollection->AddTool(Tool);
        }

        return FReply::Handled();
    }
    return FReply::Unhandled();
}*/

int32 SOdysseyPainterEditorToolTile::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
        int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    int32 RetLayer = SCompoundWidget::OnPaint(
        Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

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


void
SOdysseyPainterEditorToolTile::OnToolCheckStateChanged(ECheckBoxState InValue, UOdysseyPainterEditorTool* iTool)
{
/*
    if (InValue == ECheckBoxState::Checked)
        mOnToolSelected.ExecuteIfBound(iTool);*/
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
