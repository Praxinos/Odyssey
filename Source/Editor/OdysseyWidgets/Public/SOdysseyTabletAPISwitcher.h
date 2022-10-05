// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyStylusInputDriver.h"

/**
 * About screen contents widget
 */
class ODYSSEYWIDGETS_API SOdysseyTabletAPISwitcher
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SOdysseyTabletAPISwitcher )
    {
    }
    SLATE_END_ARGS()

    /**
     * Constructs the about screen widgets
     */
    void Construct( const FArguments& iArgs );

    static void Open();

    TSharedPtr<EOdysseyStylusInputDriver>       TabletAPISelected();

private:
    TSharedRef<SWidget> GenerateTabletAPIComboBoxItem( TSharedPtr<EOdysseyStylusInputDriver> iItem );
    void ChangeSelectionTabletAPIComboBoxItem( TSharedPtr<EOdysseyStylusInputDriver> iNewSelection, ESelectInfo::Type iSelectInfo );
    FText GetComboBoxTabletAPISelectedAsText() const;

private:
    TSharedPtr<EOdysseyStylusInputDriver>       mTabletAPISelected;
    TArray<TSharedPtr<EOdysseyStylusInputDriver>> mOptions;

};
