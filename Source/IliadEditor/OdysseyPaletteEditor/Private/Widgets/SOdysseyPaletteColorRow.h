// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "SOdysseyPaletteEntryRow.h"

/**
 * Implements a layer row widget
 */
class SOdysseyPaletteColorRow
    : public SOdysseyPaletteEntryRow
{
    SLATE_BEGIN_ARGS(SOdysseyPaletteColorRow)
        {}
        SLATE_ARGUMENT(bool, IsReadOnly)
        SLATE_ATTRIBUTE(FGuid, Set)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& iArgs
                 , const TSharedRef<SOdysseyPaletteTreeView>& iTreeView
                 , class UOdysseyPaletteEntryColor* iColorEntry);

protected:
    virtual const FSlateBrush* GetIcon() const override;

private:
    virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& InColumnName ) override;
    TSharedRef<SWidget> GenerateColorWidget();
    FReply HandleEntryColorMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
    void OnSetColorFromColorPicker(FLinearColor iNewColor);
    FLinearColor GetEntryColorAsLinear() const;

private:
    class UOdysseyPaletteEntryColor* mColorEntry;
    TSharedPtr<SWidget> mColorWidget;
    TAttribute<FGuid> mSet;
};
