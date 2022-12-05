// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyLayerExpanderArrow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SOdysseyLayerRow.h"

SLATE_IMPLEMENT_WIDGET(SOdysseyLayerExpanderArrow)
void SOdysseyLayerExpanderArrow::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, ShouldDrawWires, EInvalidateWidgetReason::Paint);
}

SOdysseyLayerExpanderArrow::SOdysseyLayerExpanderArrow()
	: ShouldDrawWires(*this, false)
{
}

void
SOdysseyLayerExpanderArrow::Construct( const FArguments& InArgs, const TSharedPtr<SOdysseyLayerRow>& iLayerRow )
{
	StyleSet = InArgs._StyleSet;
	IndentAmount = InArgs._IndentAmount;
	BaseIndentLevel = InArgs._BaseIndentLevel;
	ShouldDrawWires.Assign(*this, InArgs._ShouldDrawWires);
	mExpanderImageOpened = InArgs._ExpanderImageOpened;
	mExpanderImageClosed = InArgs._ExpanderImageClosed;
	mArrowPadding = InArgs._ArrowPadding;

	mLayerRow = iLayerRow;
	
	//Copied from SOdysseyLayerExpanderArrow (we only change VAlign to put the button at the top of the widget
	this->ChildSlot
	.Padding(TAttribute<FMargin>(this, &SOdysseyLayerExpanderArrow::GetExpanderPadding))
	[
		SAssignNew(ExpanderArrow, SButton)
		.ButtonStyle(FCoreStyle::Get(), "NoBorder")
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Center)
		//.Visibility(this, &SOdysseyLayerExpanderArrow::GetExpanderVisibility)
		.ClickMethod(EButtonClickMethod::MouseDown)
		.OnClicked(this, &SOdysseyLayerExpanderArrow::OnArrowClicked)
		.ContentPadding(0.f)
		.ForegroundColor(FSlateColor::UseForeground())
		.IsFocusable(false)
		[
			SNew(SImage)
			.Image(this, &SOdysseyLayerExpanderArrow::GetExpanderImage)
			.ColorAndOpacity(FSlateColor::UseSubduedForeground())
		]
	];
}

/** @return Visible when has children; invisible otherwise */
/* EVisibility
SOdysseyLayerExpanderArrow::GetExpanderVisibility() const
{	
	return mLayerStackNodeRow.Pin()->mLayerNode->CanHaveChildren ? EVisibility::Visible : EVisibility::Hidden;
} */


int32
SOdysseyLayerExpanderArrow::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{

	// We want to support drawing wires for the tree
	//                 Needs Wire Array
	//   v-[A]         {}
	//   |-v[B]        {1}
	//   | '-v[B]      {1,1}
	//   |   |--[C]    {1,0,1}
	//   |   |--[D]    {1,0,1}
	//   |   '--[E]    {1,0,1} 
	//   |>-[F]        {}
	//   '--[G]        {}
	//   
	//

	static const FName NAME_VerticalBarBrush = TEXT("WhiteBrush");
	const float Indent = IndentAmount.Get(10.f);
	const FSlateBrush* VerticalBarBrush = (StyleSet == nullptr) ? nullptr : StyleSet->GetBrush(NAME_VerticalBarBrush);

	if (ShouldDrawWires.Get() == true && VerticalBarBrush != nullptr)
	{
		const TSharedPtr<ITableRow> OwnerRow = mLayerRow.Pin();
		FLinearColor WireTint = InWidgetStyle.GetForegroundColor();
		WireTint.A = 0.15f;

		FMargin arrowPadding = GetExpanderPadding();
		
		static const float wireThickness = 2.0f;
		static const float distanceFromArrow = 1.f;
		const FVector2D arrowSize(16.f, 16.f);
		const FVector2D arrowPos(arrowPadding.Left, arrowPadding.Top);

		// Draw vertical wires to indicate paths to parent nodes.
		const TBitArray<>& NeedsWireByLevel = OwnerRow->GetWiresNeededByDepth();
		const int32 NumLevels = NeedsWireByLevel.Num();
		const float PreviousIndent = Indent * (NumLevels - 2);

		for (int32 Level = 1; Level < NumLevels; ++Level)
		{
			const float LevelIndent = Indent * (Level - 1);

			if (NeedsWireByLevel[Level])
			{
				//Vertical full height wire
				float posX = LevelIndent + arrowSize.X * 0.5f - wireThickness * 0.5f;
				FVector2D boxSize(wireThickness, AllottedGeometry.Size.Y);
				FVector2D boxPos(posX, 0);
				FSlateDrawElement::MakeBox(
					OutDrawElements,
					LayerId,
					AllottedGeometry.ToPaintGeometry(boxSize, FSlateLayoutTransform(boxPos)),
					VerticalBarBrush,
					ESlateDrawEffect::None,
					WireTint
				);
			}
		}

		// For items that are the last expanded child in a list, we need to draw a special angle connector wire.
		const bool bIsLastChild = OwnerRow->IsLastChild();
		if ( NumLevels > 1 && bIsLastChild )
		{
			//Previous indent vertical half arrow height wire + half of a wireThickness
			float posX = PreviousIndent + arrowSize.X  * 0.5f - wireThickness  * 0.5f;
			FVector2D boxSize(wireThickness, arrowPos.Y + arrowSize.Y * 0.5f + wireThickness * 0.5f);
			FVector2D boxPos(posX, 0);
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry(boxSize, FSlateLayoutTransform(boxPos)),
				VerticalBarBrush,
				ESlateDrawEffect::None,
				WireTint
			);
		}

		// If this item is expanded, we need to draw a 1/2-height the line down to its first child cell.
		if ( const bool bItemAppearsExpanded = OwnerRow->IsItemExpanded() && OwnerRow->DoesItemHaveChildren())
		{
			//vertical wire under the arrow + half of a wireThickness
			float posX = arrowPos.X + arrowSize.X  * 0.5f - wireThickness  * 0.5f;
			float posY = arrowPos.Y + arrowSize.Y + distanceFromArrow;
			FVector2D boxSize(wireThickness, AllottedGeometry.Size.Y - posY);
			FVector2D boxPos(posX, posY);
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry(boxSize, FSlateLayoutTransform(boxPos)),
				VerticalBarBrush,
				ESlateDrawEffect::None,
				WireTint
			);
		}

		// Draw horizontal connector from parent wire to child.
		if(NumLevels > 1)
		{
			//Additional width to occupy space of a hidden arrow
			float posX = PreviousIndent + arrowSize.X  * 0.5f + wireThickness  * 0.5f;
			float posY = arrowPos.Y + arrowSize.Y * 0.5f - wireThickness  * 0.5f;
			FVector2D boxSize(AllottedGeometry.Size.X - posX - arrowSize.X - distanceFromArrow, wireThickness);
			FVector2D boxPos(posX, posY);

			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry(boxSize, FSlateLayoutTransform(boxPos)),
				VerticalBarBrush,
				ESlateDrawEffect::None,
				WireTint
			);
		}	
	}

	LayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	return LayerId;
}

/** Invoked when the expanded button is clicked (toggle item expansion) */
FReply SOdysseyLayerExpanderArrow::OnArrowClicked()
{
	// Recurse the expansion if "shift" is being pressed
	const FModifierKeysState ModKeyState = FSlateApplication::Get().GetModifierKeys();
	if(ModKeyState.IsShiftDown())
	{
		mLayerRow.Pin()->Private_OnExpanderArrowShiftClicked();
	}
	else
	{
		mLayerRow.Pin()->ToggleExpansion();
	}

	return FReply::Handled();
}

/** @return the margin corresponding to how far this item is indented */
FMargin SOdysseyLayerExpanderArrow::GetExpanderPadding() const
{
	const int32 NestingDepth = FMath::Max(0, mLayerRow.Pin()->GetIndentLevel() - BaseIndentLevel.Get());
	const float Indent = IndentAmount.Get(10.f);
	return FMargin( NestingDepth * Indent, 0,0,0 ) + mArrowPadding;
}

/** @return the name of an image that should be shown as the expander arrow */
const FSlateBrush* SOdysseyLayerExpanderArrow::GetExpanderImage() const
{
	const bool bIsItemExpanded = mLayerRow.Pin()->IsItemExpanded();
	
	return bIsItemExpanded && mLayerRow.Pin()->GetLayer()->CanHaveChildren ? mExpanderImageOpened : mExpanderImageClosed;
}