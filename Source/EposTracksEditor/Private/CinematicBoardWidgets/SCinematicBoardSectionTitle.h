// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "Widgets/SCompoundWidget.h"

//---

class FCinematicBoardSection;
class SInlineEditableTextBlock;

class EPOSTRACKSEDITOR_API SCinematicBoardSectionTitle
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionTitle )
        {}
    SLATE_END_ARGS()

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

    // SWidget overrides
    virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

public:
    virtual void EnterRename();

protected:
    FText           HandleText() const;
    FLinearColor    HandleTextColor() const;

private:
    TWeakPtr<FCinematicBoardSection> mBoardSection;

    TSharedPtr<SInlineEditableTextBlock> mWidgetName;
};
