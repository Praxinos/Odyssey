// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Animation/Timeline/Cells/SOdysseyAnimationCell.h"
#include "OdysseyLayerCell.h"
#include "OdysseyPainterEditorAnimationProjectSettings.h"
#include "OdysseyStyle.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyLayerCellSelection.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationCell::Construct(
    const FArguments& InArgs,
    UOdysseyAnimationLayer* iAnimationLayer,
    UOdysseyLayerCell* iCell
)
{
    ensure(iAnimationLayer);

    mTimelinePosition = InArgs._TimelinePosition;

    mAnimationLayer = iAnimationLayer;
    mCell = iCell;

    ChildSlot
    [
        SNew( SBorder )
        // If Padding is changed, also modify it in OnPaint()
        .BorderImage( FOdysseyStyle::GetBrush( "FlipbookTimeline.TimelineFrameBackground" ) )
        .BorderBackgroundColor( FLinearColor( 1.f, 1.f, 1.f ) )
        .Visibility_Lambda( [this]() -> EVisibility
                            {
                                return mCell->GetExposure() > 0 ? EVisibility::Visible : EVisibility::Collapsed;
                            } )
        [
            InArgs._Content.Widget
        ]
    ];
}

int32 SOdysseyAnimationCell::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    // Draw a current frame
    LayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
    ++LayerId;

    const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );

    const float height = AllottedGeometry.GetLocalSize().Y;
    const float width = AllottedGeometry.GetLocalSize().X;

    auto FrameToPosition = [this]( int32 iFrameInCell )
        {
            // No need to manage padding and offset as the reference is the cell itself (0 = always start of the cell)
            return iFrameInCell * mTimelinePosition->GetFrameSize() /*+ mTimelinePosition->GetPadding() - mTimelinePosition->GetOffset() * mTimelinePosition->GetFrameSize()*/;
        };

    float border_padding = 2.f; // Padding of the SBorder

    int32 frame_start = mCell->GetFrameRange().GetLowerBoundValue();
    int32 frame_end = mCell->GetFrameRange().GetUpperBoundValue();

    for( int32 frame_in_layer = frame_start; frame_in_layer <= frame_end; frame_in_layer++ )
    {
        int32 frame_in_cell = frame_in_layer - frame_start;

        FIntRect frame_rect( FrameToPosition( frame_in_cell ), border_padding, FrameToPosition( frame_in_cell ) + mTimelinePosition->GetFrameSize(), height - border_padding );

        if( frame_in_layer == frame_start )
            frame_rect.Min.X += border_padding;
        if( frame_in_layer == frame_end )
            frame_rect.Max.X -= border_padding;

        //DEBUG
        //static TMap<int32, FLinearColor> map;

        //if( !map.Contains( frame_in_cell ) )
        //    map.Add( frame_in_cell, FLinearColor::MakeRandomColor() );
        //FLinearColor markColor = map.FindChecked( frame_in_cell );

        //FSlateDrawElement::MakeBox(
        //    OutDrawElements,
        //    LayerId,
        //    AllottedGeometry.ToPaintGeometry( frame_rect.Size(), FSlateLayoutTransform( frame_rect.Min ) ),
        //    GenericBrush,
        //    ESlateDrawEffect::None,
        //    markColor
        //);

        if( IsMarkSymbol( frame_in_cell ) )
        {
            FLinearColor markColor = GetMarkColor( frame_in_cell );
            markColor.A = GetMarkOpacity( frame_in_cell );
            const FSlateBrush* markBrush = GetMarkBrush( frame_in_cell );

            FSlateDrawElement::MakeBox(
                OutDrawElements,
                LayerId,
                AllottedGeometry.ToPaintGeometry( FVector2D::Min( frame_rect.Size(), markBrush->ImageSize ), FSlateLayoutTransform( frame_rect.Min ) ),
                markBrush,
                ESlateDrawEffect::None,
                markColor
            );
        }

        if( IsMarkFill( frame_in_cell ) )
        {
            FLinearColor markColor = GetMarkColor( frame_in_cell );
            markColor.A = GetMarkOpacity( frame_in_cell );

            FSlateDrawElement::MakeBox(
                OutDrawElements,
                LayerId,
                AllottedGeometry.ToPaintGeometry( frame_rect.Size(), FSlateLayoutTransform( frame_rect.Min ) ),
                GenericBrush,
                ESlateDrawEffect::None,
                markColor
            );
        }
    }

    //==========Selection============
    if(IsSelected())
    {
        FIntRect frame_rect( border_padding, border_padding, width - border_padding, height - border_padding );

        FLinearColor selectionColor = FLinearColor::Green;
        selectionColor.A = 0.2f;
        FLinearColor selectionBorderColor = FLinearColor::Green;
        selectionBorderColor.A = 1.f;

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry( frame_rect.Size(), FSlateLayoutTransform( frame_rect.Min ) ),
            GenericBrush,
            ESlateDrawEffect::None,
            selectionColor
        );

        if (IsSelectionCursor())
        {
            TArray<FVector2f> points;
            points.Add( FVector2f( 0.f, 0.f ) );
            points.Add( FVector2f( frame_rect.Width(), 0.f ) );
            points.Add( FVector2f( frame_rect.Width(), frame_rect.Height() ) );
            points.Add( FVector2f( 0.f, frame_rect.Height() ) );
            points.Add( FVector2f( 0.f, 0.f ) );

            FSlateDrawElement::MakeLines(
                OutDrawElements,
                LayerId,
                AllottedGeometry.ToPaintGeometry( frame_rect.Size(), FSlateLayoutTransform( frame_rect.Min ) ),
                points,
                ESlateDrawEffect::None,
                selectionBorderColor,
                true,
                1.5f
            );
        }
    }

    return LayerId;
}

bool
SOdysseyAnimationCell::IsSelected() const
{
    return mCell->GetLayerStack()->GetCellSelection()->GetSelectedCells().Contains(mCell);
}

bool
SOdysseyAnimationCell::IsSelectionCursor() const
{
    return mCell->GetLayerStack()->GetCellSelection()->GetCellSelectionCursor() == mCell;
}

const FSlateBrush*
SOdysseyAnimationCell::GetMarkBrush( int32 iFrameInCell ) const
{
    if( !mCell->GetMarks().Contains( iFrameInCell ) )
        return nullptr;

    int32 mark_index = mCell->GetMarks()[iFrameInCell].Index;

    UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[mark_index];
    const FSlateBrush* icon = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );
    switch(markSettings.Symbol)
    {
        case EOdysseyAnimationCellMarkSymbol::Triangle: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Triangle"); break;
        case EOdysseyAnimationCellMarkSymbol::FilledTriangle: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Filled.Triangle"); break;
        case EOdysseyAnimationCellMarkSymbol::Circle: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Circle"); break;
        case EOdysseyAnimationCellMarkSymbol::FilledCircle: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Filled.Circle"); break;
        case EOdysseyAnimationCellMarkSymbol::Diamond: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Diamond"); break;
        case EOdysseyAnimationCellMarkSymbol::FilledDiamond: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Filled.Diamond"); break;
        case EOdysseyAnimationCellMarkSymbol::Star: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Star"); break;
        case EOdysseyAnimationCellMarkSymbol::FilledStar: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Filled.Star"); break;
        case EOdysseyAnimationCellMarkSymbol::Cross: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Cross"); break;
        case EOdysseyAnimationCellMarkSymbol::Checkmark: icon = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Checkmark"); break;
    }
    return icon;
}

float
SOdysseyAnimationCell::GetMarkOpacity( int32 iFrameInCell ) const
{
    if( !mCell->GetMarks().Contains( iFrameInCell ) )
        return 0.f;

    int32 mark_index = mCell->GetMarks()[iFrameInCell].Index;

    UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[mark_index];
    if (markSettings.Symbol != EOdysseyAnimationCellMarkSymbol::Fill)
        return 1.f;

    return settings->AnimationCellsMarksFillOpacity / 100.f;
}

FLinearColor
SOdysseyAnimationCell::GetMarkColor( int32 iFrameInCell ) const
{
    if( !mCell->GetMarks().Contains( iFrameInCell ) )
        return FLinearColor();

    int32 mark_index = mCell->GetMarks()[iFrameInCell].Index;

    UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[mark_index];
    FLinearColor color = markSettings.Color;
    color.A = settings->AnimationCellsMarksFillOpacity / 100.f;

    return color;
}

bool
SOdysseyAnimationCell::IsMarkSymbol( int32 iFrameInCell ) const
{
    if( !mCell->GetMarks().Contains( iFrameInCell ) )
        return false;

    int32 mark_index = mCell->GetMarks()[iFrameInCell].Index;

    UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[mark_index];
    return markSettings.Symbol != EOdysseyAnimationCellMarkSymbol::Fill;
}

bool
SOdysseyAnimationCell::IsMarkFill( int32 iFrameInCell ) const
{
    if( !mCell->GetMarks().Contains( iFrameInCell ) )
        return false;

    int32 mark_index = mCell->GetMarks()[iFrameInCell].Index;

    UOdysseyPainterEditorAnimationProjectSettings* settings = UOdysseyPainterEditorAnimationProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[mark_index];
    return markSettings.Symbol == EOdysseyAnimationCellMarkSymbol::Fill;
}

#undef LOCTEXT_NAMESPACE
