// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STileView.h"

#include "Tools/OdysseyPainterEditorTool.h"

class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorToolsTileView
    : public SCompoundWidget
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
    void OnToolCheckStateChanged(ECheckBoxState InValue, UOdysseyPainterEditorTool* iTool);
    EVisibility ToolVisibility(UOdysseyPainterEditorTool* iTool) const;
    ECheckBoxState IsToolChecked(UOdysseyPainterEditorTool* iTool) const;
    FText ToolTooltip(UOdysseyPainterEditorTool* iTool) const;

private:
    TArray<UOdysseyPainterEditorTool*> mTools;
    FOnToolSelected mOnToolSelected;
};
