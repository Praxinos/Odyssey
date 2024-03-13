// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Cells/SOdysseyAnimationCell.h"

void
SOdysseyAnimationCell::Construct(
    const FArguments& InArgs,
    FOdysseyAnimationEditorExtension* iExtension,
    UOdysseyAnimationLayer* iAnimationLayer,
    TSharedPtr<FOdysseyAnimationCell> iCell
)
{
    ensure(iAnimationLayer);

    mExtension = iExtension;
    mAnimationLayer = iAnimationLayer;
    mCell = iCell;
    
    ChildSlot
    [
        InArgs._Content.Widget
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

	FLinearColor color = FLinearColor::Green;
	color.A = 0.2f;
    FLinearColor lineColor = FLinearColor::Green;
	lineColor.A = 1.f;

	if(IsSelected())
	{
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
        	AllottedGeometry.ToPaintGeometry( FVector2D(width, height), FSlateLayoutTransform() ),
			GenericBrush,
			ESlateDrawEffect::None,
			color
		);

        if (IsSelectionCursor())
        {
            TArray<FVector2f> points;
            points.Add(FVector2f(0.f, 0.f));
            points.Add(FVector2f(width, 0.f));
            points.Add(FVector2f(width, height));
            points.Add(FVector2f(0.f, height));
            points.Add(FVector2f(0.f, 0.f));

            FSlateDrawElement::MakeLines(
                OutDrawElements,
                LayerId,
                AllottedGeometry.ToPaintGeometry( FVector2D(width, height), FSlateLayoutTransform() ),
                points,
                ESlateDrawEffect::None,
                lineColor,
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
	return mExtension->Timeline()->GetSelectedCells().Contains(mCell);
}

bool
SOdysseyAnimationCell::IsSelectionCursor() const
{
	return mExtension->Timeline()->GetCellSelectionCursor() == mCell;
}