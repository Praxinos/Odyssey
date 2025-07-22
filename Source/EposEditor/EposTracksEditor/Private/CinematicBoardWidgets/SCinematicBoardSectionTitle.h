// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
        SLATE_ATTRIBUTE( EVisibility, OptionalWidgetsVisibility )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

public:
    virtual void EnterRename();

protected:
    FText           HandleTitleText() const;
    FSlateColor     HandleTitleTextColor() const;

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;
    TAttribute<EVisibility>             mOptionalWidgetsVisibility;

    TSharedPtr<SInlineEditableTextBlock> mWidgetName;
};
