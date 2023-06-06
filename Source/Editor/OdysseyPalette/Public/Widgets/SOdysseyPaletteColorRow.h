// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "SOdysseyPaletteEntryRow.h"

/**
 * Implements a layer row widget
 */
class ODYSSEYPALETTE_API SOdysseyPaletteColorRow
    : public SOdysseyPaletteEntryRow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyPaletteColorRow)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& iArgs, const TSharedRef<SOdysseyPaletteTreeView>& iOwnerTableView, class UOdysseyPaletteEntryColor* iColorEntry);
    
private:
    virtual TSharedRef<SWidget> GenerateHeaderWidget() override;
    FReply HandleEntryColorMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
    void OnSetColorFromColorPicker(FLinearColor iNewColor);
    FLinearColor GetEntryColor() const;

private:
    class UOdysseyPaletteEntryColor* mColorEntry;
    TSharedPtr<SWidget> mColorWidget;
};
