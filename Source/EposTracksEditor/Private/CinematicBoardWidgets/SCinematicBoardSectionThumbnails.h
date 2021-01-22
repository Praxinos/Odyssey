// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "Widgets/SCompoundWidget.h"

//---

class FCinematicBoardSection;

class EPOSTRACKSEDITOR_API SCinematicBoardSectionThumbnails
    : public SCompoundWidget
{
public:
    static float GetHeight( TSharedRef<const FCinematicBoardSection> iBoardSection );

public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionThumbnails )
        {}
    SLATE_END_ARGS()

    // Construct the widget
    void Construct(const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection);
    virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

public:

protected:
    FOptionalSize GetHeight() const;

private:
    TSharedPtr<FCinematicBoardSection>    mBoardSection;
};
