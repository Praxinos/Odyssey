// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STableRow.h"

class ODYSSEYPALETTE_API SOdysseyPaletteSetTile
    : public STableRow<FName>
{

public:
    DECLARE_DELEGATE_OneParam( FOnSetSelected, FName);

public:
    SLATE_BEGIN_ARGS(SOdysseyPaletteSetTile)
        {}
        SLATE_ARGUMENT(FName, UsedSet )
        SLATE_EVENT( FSimpleDelegate, OnSelected )
    SLATE_END_ARGS()
    
public:
    // Construction / Destruction
    ~SOdysseyPaletteSetTile();
    SOdysseyPaletteSetTile();
    
    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

public:
    FReply OnClicked();
    bool IsEnabled() const;

private:
    FSimpleDelegate mOnSelected;
    FName mUsedSet;
};
