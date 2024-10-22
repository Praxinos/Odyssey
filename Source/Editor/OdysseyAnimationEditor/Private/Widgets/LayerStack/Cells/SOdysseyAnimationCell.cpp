// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Cells/SOdysseyAnimationCell.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "AnimationEditor/OdysseyAnimationEditorProjectSettings.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationCell::Construct(
    const FArguments& InArgs,
    UOdysseyAnimationLayer* iAnimationLayer,
    UOdysseyAnimationCell* iCell
)
{
    ensure(iAnimationLayer);

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

    //==========Selection============
    if (IsMarkSymbol())
    {
        FLinearColor markColor = GetMarkColor();
        markColor.A = GetMarkOpacity();
        const FSlateBrush* markBrush = GetMarkBrush();
        FVector2D markPosition(0, height - markBrush->ImageSize.Y);

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
        	AllottedGeometry.ToPaintGeometry( markBrush->ImageSize, FSlateLayoutTransform(markPosition) ),
			markBrush,
			ESlateDrawEffect::None,
			markColor
		);
    }

    /* if (IsMarkInvalid())
    {
        const FSlateBrush* markBrush = FOdysseyStyle::GetBrush("Animation.CellMark.Symbol.Invalid");;
        FVector2D markPosition(0, height - markBrush->ImageSize.Y);

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
        	AllottedGeometry.ToPaintGeometry( markBrush->ImageSize, FSlateLayoutTransform(markPosition) ),
			markBrush,
			ESlateDrawEffect::None,
			FLinearColor::White
		);
    } */

    if (IsMarkFill())
    {
        FLinearColor markColor = GetMarkColor();
        markColor.A = GetMarkOpacity();

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
        	AllottedGeometry.ToPaintGeometry( FVector2D(width, height), FSlateLayoutTransform() ),
			GenericBrush,
			ESlateDrawEffect::None,
			markColor
		);
    }

	if(IsSelected())
	{
        FLinearColor selectionColor = FLinearColor::Green;
        selectionColor.A = 0.2f;
        FLinearColor selectionBorderColor = FLinearColor::Green;
        selectionBorderColor.A = 1.f;

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
        	AllottedGeometry.ToPaintGeometry( FVector2D(width, height), FSlateLayoutTransform() ),
			GenericBrush,
			ESlateDrawEffect::None,
			selectionColor
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
SOdysseyAnimationCell::GetMarkBrush() const
{
    if (mCell->Mark == INDEX_NONE)
        return nullptr;
    
    UOdysseyAnimationEditorProjectSettings* settings = UOdysseyAnimationEditorProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[mCell->Mark];
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
SOdysseyAnimationCell::GetMarkOpacity() const
{
    if (mCell->Mark == INDEX_NONE)
        return 0.f;
    
    UOdysseyAnimationEditorProjectSettings* settings = UOdysseyAnimationEditorProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[mCell->Mark];
    if (markSettings.Symbol != EOdysseyAnimationCellMarkSymbol::Fill)
        return 1.f;

    return settings->AnimationCellsMarksFillOpacity / 100.f;
}

FLinearColor
SOdysseyAnimationCell::GetMarkColor() const
{
    if (mCell->Mark == INDEX_NONE)
        return FLinearColor();
    
    UOdysseyAnimationEditorProjectSettings* settings = UOdysseyAnimationEditorProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[mCell->Mark];
    FLinearColor color = markSettings.Color;
    color.A = settings->AnimationCellsMarksFillOpacity / 100.f;

    return color;
}

bool
SOdysseyAnimationCell::IsMarkSymbol() const
{
    if (mCell->Mark == INDEX_NONE)
        return false;
    
    UOdysseyAnimationEditorProjectSettings* settings = UOdysseyAnimationEditorProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[mCell->Mark];
    return markSettings.Symbol != EOdysseyAnimationCellMarkSymbol::Fill;
}

bool
SOdysseyAnimationCell::IsMarkFill() const
{
    if (mCell->Mark == INDEX_NONE)
        return false;
    
    UOdysseyAnimationEditorProjectSettings* settings = UOdysseyAnimationEditorProjectSettings::Get();
    const FAnimationCellMarkSettings& markSettings = settings->AnimationCellsMarks[mCell->Mark];
    return markSettings.Symbol == EOdysseyAnimationCellMarkSymbol::Fill;
}

#undef LOCTEXT_NAMESPACE