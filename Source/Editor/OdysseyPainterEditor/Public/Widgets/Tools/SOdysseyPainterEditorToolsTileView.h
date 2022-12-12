// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STileView.h"

class UOdysseyPainterEditorTool;

class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorToolsTileView
    : public STileView<UOdysseyPainterEditorTool*>
{
public:
    DECLARE_DELEGATE_OneParam( FOnToolSelected, UOdysseyPainterEditorTool* );

public:
    SLATE_BEGIN_ARGS(SOdysseyPainterEditorToolsTileView)
        {}
        SLATE_ARGUMENT( TArray<UOdysseyPainterEditorTool*>, Tools )
        SLATE_EVENT( FOnToolSelected, OnToolSelected )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPainterEditorToolsTileView();
    SOdysseyPainterEditorToolsTileView();
    
    void Construct(const FArguments& InArgs);
    
private:
	TSharedRef<class ITableRow> OnGenerateTile(UOdysseyPainterEditorTool* iTool, const TSharedRef< class STableViewBase >& iTable);
    void OnToolSelected(UOdysseyPainterEditorTool* iTool);

private:
    TArray<UOdysseyPainterEditorTool*> mTools;
    FOnToolSelected mOnToolSelected;
};
