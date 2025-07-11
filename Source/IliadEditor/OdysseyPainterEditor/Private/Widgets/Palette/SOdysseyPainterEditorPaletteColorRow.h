// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Palette/SOdysseyPainterEditorPaletteEntryRow.h"

class UOdysseyPaletteEntryColor;

/**
 * Implements a layer row widget
 */
class SOdysseyPainterEditorPaletteColorRow
    : public SOdysseyPainterEditorPaletteEntryRow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyPainterEditorPaletteColorRow)
        : _IsCurrent( false )
        , _Entry(nullptr)
        , _Set(FString())
        {}
        SLATE_ATTRIBUTE(bool, IsCurrent)
        SLATE_ATTRIBUTE(UOdysseyPaletteEntryColor*, Entry)
        SLATE_ATTRIBUTE(FString, Set)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& iArgs, const TSharedRef<STableViewBase>& iTreeView);

protected:
    virtual const FSlateBrush* GetIcon() const override;

public:
    /** @return True if the corresponding item is selected; false otherwise */
    virtual bool IsItemSelected() const override;

private:
    virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& InColumnName ) override;
    TSharedRef<SWidget> GenerateColorWidget();
    FLinearColor GetEntryColorAsLinear() const;

private:
    TAttribute<bool> mIsCurrent;
    TAttribute<FString> mSet;
    TAttribute<UOdysseyPaletteEntryColor*> mEntryColor;
};
