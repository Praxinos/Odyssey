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
        SLATE_ATTRIBUTE( EVisibility, OptionalWidgetsVisibility )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

public:
    virtual void EnterRename();

protected:
    FText           HandleText() const;
    FLinearColor    HandleTextColor() const;

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;
    TAttribute<EVisibility>             mOptionalWidgetsVisibility;

    TSharedPtr<SInlineEditableTextBlock> mWidgetName;
};
