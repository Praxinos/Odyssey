// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STableRow.h"

class UOdysseyPainterEditorTool;

class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorToolTile
    : public STableRow<UOdysseyPainterEditorTool*>
{
public:
    DECLARE_DELEGATE_OneParam( FOnToolSelected, UOdysseyPainterEditorTool* );

public:
    SLATE_BEGIN_ARGS(SOdysseyPainterEditorToolTile)
        {}
        SLATE_ARGUMENT( UOdysseyPainterEditorTool*, Tool )
        SLATE_EVENT( FSimpleDelegate, OnSelected )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPainterEditorToolTile();
    SOdysseyPainterEditorToolTile();
    
    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

public:
    FReply OnClicked();

private:
    FSimpleDelegate mOnSelected;
};
