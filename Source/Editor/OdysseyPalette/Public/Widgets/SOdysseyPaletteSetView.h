// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STileView.h"

class ODYSSEYPALETTE_API SOdysseyPaletteSetView
    : public STileView<FName>
{

public:
    DECLARE_DELEGATE_OneParam( FOnSetSelected, FName);

public:
    SLATE_BEGIN_ARGS(SOdysseyPaletteSetView)
        {}
        SLATE_ARGUMENT( UOdysseyPalette*, Palette )
        SLATE_EVENT( FOnSetSelected, OnSetSelected )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPaletteSetView();
    SOdysseyPaletteSetView();
    
    void Construct(const FArguments& InArgs);
    void OnSetSelected(FName iSet);

private:
	TSharedRef<class ITableRow> OnGenerateTile(FName iSet, const TSharedRef< class STableViewBase >& iTable);

private:
    UOdysseyPalette* mPalette;
    FOnSetSelected mOnSetSelected;
};
