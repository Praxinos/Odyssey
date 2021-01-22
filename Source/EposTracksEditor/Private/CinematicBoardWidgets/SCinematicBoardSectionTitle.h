// IDDN FR.001.250001.004.S.X.2019.000.00000
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
    static float GetHeight( TSharedRef<const FCinematicBoardSection> iBoardSection );

public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionTitle )
        : _Name()
        {}
        SLATE_ATTRIBUTE( FText, Name )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

public:
    virtual void EnterRename();

private:
    TSharedPtr<FCinematicBoardSection> mBoardSection;

    TSharedPtr<SInlineEditableTextBlock> mWidgetName;

    TAttribute<FText> mName;
};
