// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SCompoundWidget.h"

/**
 * Expander arrow and indentation component that can be placed in a TableRow
 * of a TreeView. Intended for use by TMultiColumnRow in TreeViews.
 */
class ODYSSEYLAYERSTACKEDITOR_API SOdysseyLayerExpanderArrow : public SCompoundWidget
{
	SLATE_DECLARE_WIDGET(SOdysseyLayerExpanderArrow, SCompoundWidget)

public:
	SLATE_BEGIN_ARGS( SOdysseyLayerExpanderArrow )
		: _StyleSet(&FCoreStyle::Get())
		, _ExpanderImageOpened(_StyleSet->GetBrush("TreeArrow_Expanded"))
		, _ExpanderImageClosed(_StyleSet->GetBrush("TreeArrow_Collapsed"))
		, _IndentAmount(10)
		, _BaseIndentLevel(0)
		, _ShouldDrawWires(false)
	{ }
		SLATE_ARGUMENT(const ISlateStyle*, StyleSet)

		SLATE_ARGUMENT(const FSlateBrush*, ExpanderImageOpened)

		SLATE_ARGUMENT(const FSlateBrush*, ExpanderImageClosed)
		
		SLATE_ARGUMENT(FMargin, ArrowPadding)

		/** How many Slate Units to indent for every level of the tree. */
		SLATE_ATTRIBUTE(float, IndentAmount)
		/** The level that the root of the tree should start (e.g. 2 will shift the whole tree over by `IndentAmount*2`) */
		SLATE_ATTRIBUTE(int32, BaseIndentLevel)
		/** Whether to draw the wires that visually reinforce the tree hierarchy. */
		SLATE_ATTRIBUTE(bool, ShouldDrawWires)

	SLATE_END_ARGS()

	SOdysseyLayerExpanderArrow();

public:
	void Construct( const FArguments& InArgs, const TSharedPtr<class SOdysseyLayerRow>& iLayerStackNodeRow );

private:
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	/** Invoked when the expanded button is clicked (toggle item expansion) */
	FReply OnArrowClicked();

	/** @return the margin corresponding to how far this item is indented */
	FMargin GetExpanderPadding() const;

	/** @return the name of an image that should be shown as the expander arrow */
	const FSlateBrush* GetExpanderImage() const;

private:
	TWeakPtr<class SOdysseyLayerRow> mLayerRow;
	const FSlateBrush* mExpanderImageOpened;
	const FSlateBrush* mExpanderImageClosed;
	FMargin mArrowPadding;

	/** A reference to the expander button */
	TSharedPtr<SButton> ExpanderArrow;

	/** The slate style to use */
	const ISlateStyle* StyleSet;

	/** The amount of space to indent at each level */
	TAttribute<float> IndentAmount;

	/** The level in the tree that begins the indention amount */
	TAttribute<int32> BaseIndentLevel;

	/** Whether to draw the wires that visually reinforce the tree hierarchy. */
	TSlateAttribute<bool> ShouldDrawWires;
};
