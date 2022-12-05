// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STileView.h"

class UOdysseyTool;

class ODYSSEYPAINTEREDITOR_API SOdysseyToolsTileView
    : public STileView<UOdysseyTool*>
{
public:
    DECLARE_DELEGATE_OneParam( FOnToolSelected, UOdysseyTool* );

public:
    SLATE_BEGIN_ARGS(SOdysseyToolsTileView)
        {}
        SLATE_ARGUMENT( TArray<UOdysseyTool*>, Tools )
        SLATE_EVENT( FOnToolSelected, OnToolSelected )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyToolsTileView();
    SOdysseyToolsTileView();
    
    void Construct(const FArguments& InArgs);
    
private:
	TSharedRef<class ITableRow> OnGenerateTile(UOdysseyTool* iTool, const TSharedRef< class STableViewBase >& iTable);
    void OnToolSelected(UOdysseyTool* iTool);

private:
    TArray<UOdysseyTool*> mTools;
    FOnToolSelected mOnToolSelected;
};
